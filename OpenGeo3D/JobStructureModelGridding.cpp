#include "JobStructureModelGridding.h"
#include <wx/log.h>
#include <vtkTriangle.h>
#include <vtkPolyDataCollection.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkPolygon.h>
#include <g3dvtk/ObjectFactory.h>
#include "Events.h"
#include "Strings.h"

g3dgrid::VoxelGrid* JobStructureModelGridding::g3dVoxelGrid_ = nullptr;
int JobStructureModelGridding::lodLevel_ = -1;
std::vector<JobStructureModelGridding::JobThread*> JobStructureModelGridding::jobThreads_;
wxCriticalSection JobStructureModelGridding::criticalSection_;

wxString JobStructureModelGridding::NameOfFeatureType(FeatureType type) {
	switch (type) {
	case FeatureType::GeologicalBody:
		return Strings::NameOfGeologicFeatureBody();
	case FeatureType::GeologicalSamplePoint:
		return Strings::NameOfGeologicFeatureSamplePoint();
	default:
		return Strings::NameOfGeologicFeatureUnknown();
	}
}

JobStructureModelGridding::FeatureType JobStructureModelGridding::NameToFeatureType(const wxString& name) {
	if (name.CmpNoCase(Strings::NameOfGeologicFeatureBody()) == 0) {
		return FeatureType::GeologicalBody;
	} else if (name.CmpNoCase(Strings::NameOfGeologicFeatureSamplePoint()) == 0) {
		return FeatureType::GeologicalSamplePoint;
	} else {
		return FeatureType::Unknown;
	}
}

void JobStructureModelGridding::Start(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::VoxelGrid* g3dVoxelGrid, int lodLevel) {
	wxCriticalSectionLocker locker(criticalSection_);
	if (!jobThreads_.empty()) {
		return;
	}
	if (featureClasses.empty()) {
		wxLogInfo(wxS("stop gridding since no feature class is provided."));
		return;
	}
	g3dVoxelGrid_ = g3dVoxelGrid;
	lodLevel_ = lodLevel;
	g3dgrid::LOD* g3dGridLOD = g3dVoxelGrid_->GetLOD(lodLevel_);
	if (g3dGridLOD == nullptr) {
		wxLogWarning(wxS("stop gridding since no LOD exists in the VoxelGrid."));
		return;
	}
	int numberOfThreads = wxThread::GetCPUCount() - 2;
	if (numberOfThreads < 1) {
		numberOfThreads = 1;
	}
	g3dGridLOD->Extend(range);
	for (int n = 0; n < numberOfThreads; ++n) {
		JobThread* jobThread = new JobThread(featureClasses, range, g3dGridLOD, &criticalSection_, numberOfThreads, n);
		jobThreads_.push_back(jobThread);
		jobThread->Run();
	}
}

bool JobStructureModelGridding::IsRunning() {
	wxCriticalSectionLocker locker(criticalSection_);
	return !jobThreads_.empty();
}

void JobStructureModelGridding::Stop() {
	wxCriticalSectionLocker locker(criticalSection_);
	for (JobThread* job : jobThreads_) {
		job->Delete();	// wxThread::Wait() dose not ask the thread to quit, but only wait for it to quit.
		delete job;
	}
	Clear();
}

void JobStructureModelGridding::JobThreadQuit(wxThreadIdType threadId) {
	wxCriticalSectionLocker locker(criticalSection_);
	auto citor = jobThreads_.cbegin();
	while ((*citor)->GetId() != threadId && citor != jobThreads_.cend()) {
		++citor;
	}
	if (citor == jobThreads_.cend()) {
		return;
	}
	(*citor)->Delete();	// wxThread::Wait() dose not ask the thread to quit, but only wait for it to quit.
	delete (*citor);
	jobThreads_.erase(citor);
	if (jobThreads_.empty()) {
		Clear();
		wxLogInfo(Strings::MessageOfGriddingJobEnd());
	} else {
		wxLogInfo(Strings::MessageOfGriddingJobThreadStop(threadId, int(jobThreads_.size())));
	}
}

void JobStructureModelGridding::Clear() {
	jobThreads_.clear();
	g3dVoxelGrid_ = nullptr;
	lodLevel_ = -1;
	tinBufMutex_.lock();
	std::map<void*, TinBuf*>::iterator itor = tinBufs_.begin();
	while (itor != tinBufs_.end()) {
		delete itor->second;
		++itor;
	}
	tinBufs_.clear();
	tinBufMutex_.unlock();
}

JobStructureModelGridding::JobThread::JobThread(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::LOD* g3dGridLOD, wxCriticalSection* gridLODCS, int totalWokers, int workerNo) : wxThread(wxTHREAD_JOINABLE) {
	sourceFeatureClasses_ = featureClasses;
	range_ = range;
	g3dGridLOD_ = g3dGridLOD;
	gridLODCS_ = gridLODCS;
	totalWorkers_ = totalWokers;
	workerNo_ = workerNo;

	mPointAll_ = nullptr;
	mPointNet_ = nullptr;
	mPointSampling_ = nullptr;
}

JobStructureModelGridding::JobThread::~JobThread() {

}

wxThread::ExitCode JobStructureModelGridding::JobThread::Entry() {
	wxStopWatch sw;
	ExitCode code = RunGridding();
	wxThreadEvent* thdEvt = new wxThreadEvent(wxEVT_THREAD);
	wxThreadIdType thdId = GetId();
	thdEvt->SetExtraLong(thdId);
	thdEvt->SetInt(sw.Time() / 1000);
	if (TestDestroy()) {
		thdEvt->SetId(Events::ID::Thread_GriddingThreadQuit);
		Events::QueueThreadEvent(thdEvt);
	} else {
		thdEvt->SetId(Events::ID::Thread_GriddingThreadFinished);
		Events::QueueThreadEvent(thdEvt);
	}
	return code;
}

wxThread::ExitCode JobStructureModelGridding::JobThread::RunGridding() {
	geo3dml::Point3D origin, step;
	gridLODCS_->Enter();
	origin = g3dGridLOD_->GetGridOrigin();
	g3dGridLOD_->GetVoxelSize(step.x, step.y, step.z);
	gridLODCS_->Leave();
	g3dgrid::VoxelBox ijkRange = g3dgrid::VoxelGrid::BoxToVoxelBox(origin, step, range_);
	int dimI = ijkRange.max.i - ijkRange.min.i + 1;
	int dimJ = ijkRange.max.j - ijkRange.min.j + 1;
	int dimK = ijkRange.max.k - ijkRange.min.k + 1;
	int startI, endI;
	int quotient = dimI / totalWorkers_;
	int remainder = dimI % totalWorkers_;
	if (workerNo_ < remainder) {
		startI = ijkRange.min.i + workerNo_ * (quotient + 1);
		endI = startI + quotient + 1;
	} else {
		startI = ijkRange.min.i + remainder * (quotient + 1) + (workerNo_ - remainder) * quotient;
		endI = startI + quotient;
	}
	if (startI >= endI) {
		ExitCode(0);
	}
	const size_t batchSizeOfCells = 10000;
	// resampling geological features by half distance of the step along x, y and z axies, respectively.
	const double bottomZ = origin.z + step.z * ijkRange.min.k;
	const double topZ = origin.z + step.z * (ijkRange.max.k + 1);
	const double halfStepZ = step.z / 2;
	vtkSmartPointer<vtkPolyDataCollection> leftPillars, rightPillars;
	for (int i = startI; i < endI && !TestDestroy(); ++i) {
		double leftX = origin.x + step.x * i;
		double middleX = leftX + step.x / 2;
		double rightX = leftX + step.x;
		if (i == startI) {
			leftPillars = vtkSmartPointer<vtkPolyDataCollection>::New();
		} else {
			leftPillars = rightPillars;
		}
		rightPillars = vtkSmartPointer<vtkPolyDataCollection>::New();
		vtkSmartPointer<vtkPolyData> bottomMiddlePillar, centerPillar, topMiddlePillar;
		vtkSmartPointer<vtkPolyData> bottomRightPillar, middleRightPillar, topRightPillar;
		std::vector<g3dgrid::VoxelCell> cells;
		cells.reserve(batchSizeOfCells + batchSizeOfCells / 2);
		for (int relativeJ = 0; relativeJ < dimJ && !TestDestroy(); ++relativeJ) {
			// pillars for column of (i, j, *)
			double bottomY = origin.y + step.y * (relativeJ + ijkRange.min.j);
			double middleY = bottomY + step.y / 2;
			double topY = bottomY + step.y;
			// left
			vtkSmartPointer<vtkPolyData> bottomLeftPillar, middleLeftPillar, topLeftPillar;
			if (i == startI) {
				if (relativeJ == 0) {
					bottomLeftPillar = NewPillar(leftX, bottomY, bottomZ, topZ, halfStepZ);
				} else {
					bottomLeftPillar = vtkPolyData::SafeDownCast(leftPillars->GetItemAsObject(0));
					leftPillars->RemoveItem(0);
				}
				middleLeftPillar = NewPillar(leftX, middleY, bottomZ, topZ, halfStepZ);
				topLeftPillar = NewPillar(leftX, topY, bottomZ, topZ, halfStepZ);
				leftPillars->AddItem(topLeftPillar);
			} else {
				bottomLeftPillar = vtkPolyData::SafeDownCast(leftPillars->GetItemAsObject(0));
				middleLeftPillar = vtkPolyData::SafeDownCast(leftPillars->GetItemAsObject(1));
				topLeftPillar = vtkPolyData::SafeDownCast(leftPillars->GetItemAsObject(2));
				leftPillars->RemoveItem(0);
				leftPillars->RemoveItem(1);
			}
			// middle
			if (relativeJ == 0) {
				bottomMiddlePillar = NewPillar(middleX, bottomY, bottomZ, topZ, halfStepZ);
			} else {
				bottomMiddlePillar = topMiddlePillar;
			}
			centerPillar = NewPillar(middleX, middleY, bottomZ, topZ, halfStepZ);
			topMiddlePillar = NewPillar(middleX, topY, bottomZ, topZ, halfStepZ);
			// right
			if (relativeJ == 0) {
				bottomRightPillar = NewPillar(rightX, bottomY, bottomZ, topZ, halfStepZ);
				rightPillars->AddItem(bottomRightPillar);
			} else {
				bottomRightPillar = vtkPolyData::SafeDownCast(rightPillars->GetItemAsObject(relativeJ * 2));
			}
			middleRightPillar = NewPillar(rightX, middleY, bottomZ, topZ, halfStepZ);
			rightPillars->AddItem(middleRightPillar);
			topRightPillar = NewPillar(rightX, topY, bottomZ, topZ, halfStepZ);
			rightPillars->AddItem(topRightPillar);
			// cells at column (i, j, *)
			const int numOfLinkedSamples = 3 * 9;
			int linkedSamples[numOfLinkedSamples] = { -1 };
			size_t numOfFeatureClasses = sourceFeatureClasses_.size();
			for (size_t fcIndex = 0; fcIndex < numOfFeatureClasses && !TestDestroy(); ++fcIndex) {
				std::string propName = MakeFieldNameToFeatureClass(fcIndex);
				vtkAbstractArray* dataArray = bottomLeftPillar->GetPointData()->GetAbstractArray(propName.c_str());
				if (dataArray == NULL) {
					continue;	// skip this feature class since it was not involved in sample.
				}
				// cell values for all fields in the feature class.
				vtkIntArray* bottomLeftSamples = vtkIntArray::SafeDownCast(bottomLeftPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* middleLeftSamples = vtkIntArray::SafeDownCast(middleLeftPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* topLeftSamples = vtkIntArray::SafeDownCast(topLeftPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* bottomMiddleSamples = vtkIntArray::SafeDownCast(bottomMiddlePillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* centerSamples = vtkIntArray::SafeDownCast(centerPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* topMiddleSamples = vtkIntArray::SafeDownCast(topMiddlePillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* bottomRightSamples = vtkIntArray::SafeDownCast(bottomRightPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* middleRightSamples = vtkIntArray::SafeDownCast(middleRightPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				vtkIntArray* topRightSamples = vtkIntArray::SafeDownCast(topRightPillar->GetPointData()->GetAbstractArray(propName.c_str()));
				geo3dml::FeatureClass* fc = sourceFeatureClasses_[fcIndex].first;
				int numOfFields = fc->GetFieldCount();
				for (int relativeK = 0; relativeK < dimK && !TestDestroy(); ++relativeK) {
					g3dgrid::VoxelCell cell(i, relativeJ + ijkRange.min.j, relativeK + ijkRange.min.k);
					for (int f = 0; f < 3; ++f) {
						int sampleIndex = relativeK * 2 + f;	// 2 for half step sampling
						linkedSamples[f * 9 + 0] = bottomLeftSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 1] = middleLeftSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 2] = topLeftSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 3] = bottomMiddleSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 4] = centerSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 5] = topMiddleSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 6] = bottomRightSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 7] = middleRightSamples->GetValue(sampleIndex);
						linkedSamples[f * 9 + 8] = topRightSamples->GetValue(sampleIndex);
					}
					// empty cell test
					cell.IsValid(false);
					for (int f = 0; f < numOfLinkedSamples; ++f) {
						if (linkedSamples[f] >= 0) {
							cell.IsValid(true);
							break;
						}
					}
					for (int f = 0; f < numOfFields; ++f) {
						const geo3dml::Field& field = fc->GetFieldAt(f);
						CheckOrAddFieldIntoGrid(field);
						if (!cell.IsValid()) {
							continue;
						}
						std::string fieldName = field.Name();
						switch (field.DataType()) {
						case geo3dml::Field::ValueType::Boolean: {
							// select majority votes on TRUE or FALSE.
							std::map<bool, char> votes;
							votes[true] = 0;
							votes[false] = 0;
							for (int f = 0; f < numOfLinkedSamples; ++f) {
								if (linkedSamples[f] < 0) {
									continue;
								}
								auto* boolValue = dynamic_cast<geo3dml::BooleanFieldValue*>(fc->GetFeatureAt(linkedSamples[f])->GetField(fieldName));
								if (boolValue != nullptr) {
									votes[boolValue->Value()] += 1;
								} else {
									votes[false] += 1;
								}
							}
							auto* boolValue = new geo3dml::BooleanFieldValue(fieldName);
							boolValue->Value(votes[true] >= votes[false]);
							cell.SetField(boolValue);
							break;
						}
						case geo3dml::Field::ValueType::Double: {
							// take average value as cell's value.
							int numOfSamples = 0;
							double average = 0;
							for (int f = 0; f < numOfLinkedSamples; ++f) {
								if (linkedSamples[f] < 0) {
									continue;
								}
								auto* doubleValue = dynamic_cast<geo3dml::DoubleFieldValue*>(fc->GetFeatureAt(linkedSamples[f])->GetField(fieldName));
								if (doubleValue != nullptr) {
									++numOfSamples;
									average += doubleValue->Value();
								}
							}
							if (numOfSamples > 0) {
								average = average / numOfSamples;
							}
							auto* doubleValue = new geo3dml::DoubleFieldValue(fieldName);
							doubleValue->Value(average);
							cell.SetField(doubleValue);
							break;
						}
						case geo3dml::Field::ValueType::Integer: {
							// select the value with majority votes.
							std::map<int, char> votes;
							for (int f = 0; f < numOfLinkedSamples; ++f) {
								if (linkedSamples[f] < 0) {
									continue;
								}
								auto* intValue = dynamic_cast<geo3dml::IntegerFieldValue*>(fc->GetFeatureAt(linkedSamples[f])->GetField(fieldName));
								if (intValue != nullptr) {
									if (votes.find(intValue->Value()) != votes.cend()) {
										votes[intValue->Value()] += 1;
									} else {
										votes[intValue->Value()] = 1;
									}
								}
							}
							std::map<int, char>::const_iterator majorVote = votes.cbegin();
							std::map<int, char>::const_iterator citor = majorVote;
							while (citor != votes.cend()) {
								if (citor->second > majorVote->second) {
									majorVote = citor;
								}
								++citor;
							}
							if (majorVote != votes.cend()) {
								auto* intValue = new geo3dml::IntegerFieldValue(fieldName);
								intValue->Value(majorVote->first);
								cell.SetField(intValue);
							}
							break;
						}
						case geo3dml::Field::ValueType::Text: {
							// select the text with majority votes.
							std::map<std::string, char> votes;
							for (int f = 0; f < numOfLinkedSamples; ++f) {
								if (linkedSamples[f] < 0) {
									continue;
								}
								auto* textValue = dynamic_cast<geo3dml::TextFieldValue*>(fc->GetFeatureAt(linkedSamples[f])->GetField(fieldName));
								if (textValue != nullptr) {
									if (votes.find(textValue->Value()) != votes.cend()) {
										votes[textValue->Value()] += 1;
									} else {
										votes[textValue->Value()] = 1;
									}
								}
							}
							std::map<std::string, char>::const_iterator majorVote = votes.cbegin();
							std::map<std::string, char>::const_iterator citor = majorVote;
							while (citor != votes.cend()) {
								if (citor->second > majorVote->second) {
									majorVote = citor;
								}
								++citor;
							}
							if (majorVote != votes.cend()) {
								auto* textValue = new geo3dml::TextFieldValue(fieldName);
								textValue->Value(majorVote->first);
								cell.SetField(textValue);
							}
							break;
						}
						default:
							break;
						}
					}
					cells.push_back(cell);
				}
				if (TestDestroy()) {
					break;
				}
				if (cells.size() >= batchSizeOfCells) {
					SubmitCells(cells);
					cells.clear();
				}
			}
		}
		if (!TestDestroy() && !cells.empty()) {
			SubmitCells(cells);
		}
		cells.clear();
	}

	return ExitCode(0);
}

vtkSmartPointer<vtkPolyData> JobStructureModelGridding::JobThread::NewPillar(double x, double y, double bottomZ, double topZ, double stepZ) {
	double topPoint[3], bottomPoint[3];
	topPoint[0] = bottomPoint[0] = x;
	topPoint[1] = bottomPoint[1] = y;
	topPoint[2] = topZ;
	bottomPoint[2] = bottomZ;
	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	polyData->SetPoints(points);
	vtkIdType ptId = points->InsertNextPoint(bottomPoint);
	double z = bottomZ + stepZ;
	while (z < topZ) {
		ptId = points->InsertNextPoint(x, y, z);
		z += stepZ;	// from bottom to top
	}
	ptId = points->InsertNextPoint(topPoint);
	const vtkIdType numOfPoints = polyData->GetNumberOfPoints();
	size_t fcIndex = 0;
	const int NullFeatureIndex = -1;
	int samplingIndexBase = 0;
	geo3dml::ShapeProperty* samplingProp = NULL;
	if (mPointSampling_ != NULL) {
		samplingIndexBase = mPointSampling_->GetPointCount();
		mPointSampling_->AddPoint(bottomPoint[0], bottomPoint[1], bottomPoint[2]);
		z = bottomZ + stepZ;
		while (z < topZ) {
			mPointSampling_->AddPoint(x, y, z);
			z += stepZ;
		}
		mPointSampling_->AddPoint(topPoint[0], topPoint[1], topPoint[2]);
		samplingProp = mPointSampling_->GetProperty(geo3dml::ShapeProperty::Vertex);
		for (fcIndex = 0; fcIndex < sourceFeatureClasses_.size(); ++fcIndex) {
			std::string propName = MakeFieldNameToFeatureClass(fcIndex);
			if (samplingProp->GetFieldIndex(propName) < 0) {
				geo3dml::Field samplingFieldDef;
				samplingFieldDef.Name(propName).DataType(geo3dml::Field::Integer);
				samplingProp->AddField(samplingFieldDef);
			}
			for (int v = 0; v < numOfPoints; ++v) {
				samplingProp->IntValue(propName, samplingIndexBase + v, NullFeatureIndex);
			}
		}
	}
	struct IntersectionElement {
		double z;
		std::vector<int> triangles;
		bool isAccepted;
		IntersectionElement() {
			z = 0;
			isAccepted = true;
		};
	};
	int v1 = 0, v2 = 0, v3 = 0;
	double intersectPoint[3] = { 0 }, boundingRect[6] = { 0 };
	std::list<IntersectionElement> intersections;
	vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
	triangle->Initialize();
	topPoint[2] = range_.max.z + (range_.max.z - range_.min.z);		// extend topPoint and bottomPoint in Z to make sure the pillar go through all geometries in the model.
	bottomPoint[2] = range_.min.z - (range_.max.z - range_.min.z);
	for (fcIndex = 0; fcIndex < sourceFeatureClasses_.size() && !TestDestroy(); ++fcIndex) {
		std::string propName = MakeFieldNameToFeatureClass(fcIndex);
		vtkSmartPointer<vtkIntArray> featureLabelArray = vtkSmartPointer<vtkIntArray>::New();
		featureLabelArray->SetName(propName.c_str());
		featureLabelArray->SetNumberOfComponents(1);
		polyData->GetPointData()->AddArray(featureLabelArray);
		for (vtkIdType v = 0; v < numOfPoints; ++v) {	// init featureLabelArray to null features.
			featureLabelArray->InsertValue(v, NullFeatureIndex);
		}
		int numOfTestFeatures = 0;
		geo3dml::FeatureClass* fc = sourceFeatureClasses_[fcIndex].first;
		int numOfFeatures = fc->GetFeatureCount();
		for (int featureIndex = 0; featureIndex < numOfFeatures && !TestDestroy(); ++featureIndex) {
			geo3dml::Feature* feature = fc->GetFeatureAt(featureIndex);
			int numOfGeometries = feature->GetGeometryCount();
			if (numOfGeometries != 1) {
				continue;
			}
			geo3dml::Geometry* geo = feature->GetGeometryAt(0);
			geo3dml::TIN* tin = dynamic_cast<geo3dml::TIN*>(geo);
			if (tin == NULL) {
				continue;
			}
			++numOfTestFeatures;
			tin->GetMinimumBoundingRectangle(boundingRect[0], boundingRect[1], boundingRect[2], boundingRect[3], boundingRect[4], boundingRect[5]);
			if (topPoint[0] < boundingRect[0] || topPoint[0] > boundingRect[3] || topPoint[1] < boundingRect[1] || topPoint[1] > boundingRect[4]) {
				continue;
			}
			intersections.clear();
			const TinBuf* tinBuf = GetTinBuf(tin);
			for (int triIndex = 0; triIndex < tinBuf->numOfTriangles_ && !TestDestroy(); ++triIndex) {
				v1 = tinBuf->triangles_[triIndex * 3];
				v2 = tinBuf->triangles_[triIndex * 3 + 1];
				v3 = tinBuf->triangles_[triIndex * 3 + 2];
				triangle->GetPoints()->SetPoint(0, tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2]);
				triangle->GetPoints()->SetPoint(1, tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2]);
				triangle->GetPoints()->SetPoint(2, tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2]);
				if (!IntersectWithLine(triangle, topPoint, bottomPoint, intersectPoint)) {
					continue;
				}
				// sort intersections from bottom to top.
				std::list<IntersectionElement>::iterator itor = intersections.begin();
				while (itor != intersections.end()) {
					if (IsEqual(intersectPoint[2], itor->z)) {
						itor->triangles.push_back(triIndex);
						break;
					} else if (intersectPoint[2] < itor->z) {
						IntersectionElement xe;
						xe.z = intersectPoint[2];
						xe.triangles.push_back(triIndex);
						intersections.insert(itor, xe);
						break;
					}
					++itor;
				}
				if (itor == intersections.end()) {
					IntersectionElement xe;
					xe.z = intersectPoint[2];
					xe.triangles.push_back(triIndex);
					intersections.push_back(xe);
				}
			}
			// in the case of the vertical line intersecting mulitple triangles at one point.
			int numOfAcceptedPt = 0;
			std::list<IntersectionElement>::iterator itor = intersections.begin();
			while (itor != intersections.end()) {
				itor->isAccepted = JudgeOnInsersection(x, y, itor->z, itor->triangles, tinBuf);
				if (itor->isAccepted) {
					++numOfAcceptedPt;
				}
				++itor;
			}
			if (numOfAcceptedPt < 2) {
				continue;
			}
			// for a regularly closed body, the line should intersect it at even intersections.
			if (numOfAcceptedPt % 2 != 0) {
				// reject the middle intersection
				int counter = 0, middle = (numOfAcceptedPt + 1) / 2;
				itor = intersections.begin();
				while (itor != intersections.end()) {
					if (itor->isAccepted) {
						if (++counter == middle) {
							itor->isAccepted = false;
							break;
						}
					}
					++itor;
				}
			}
			if (mPointAll_ != NULL && mPointNet_ != NULL) {
				// for debug
				itor = intersections.begin();
				while (itor != intersections.end()) {
					mPointAll_->AddPoint(x, y, itor->z);
					if (itor->isAccepted) {
						mPointNet_->AddPoint(x, y, itor->z);
					}
					++itor;
				}
			}
			// vertices
			std::list<IntersectionElement>::const_iterator curPt = intersections.cbegin();
			while (curPt != intersections.cend() && !curPt->isAccepted) {
				++curPt;
			}
			int numOfLowerPoints = 0;
			for (vtkIdType v = 0; v < numOfPoints; ++v) {
				double* pt = polyData->GetPoint(v);
				while (curPt != intersections.cend() && (!curPt->isAccepted || pt[2] >= curPt->z)) {
					if (curPt->isAccepted) {
						++numOfLowerPoints;
					}
					++curPt;
				}
				if (numOfLowerPoints % 2 == 1) {
					featureLabelArray->InsertValue(v, featureIndex);
					if (samplingProp != NULL) {
						samplingProp->IntValue(propName, samplingIndexBase + v, featureIndex);
					}
				}
			}
		}
		if (numOfTestFeatures < 1) {
			polyData->GetPointData()->RemoveArray(featureLabelArray->GetName());
		}
	}
	return polyData;
}

std::string JobStructureModelGridding::JobThread::MakeFieldNameToFeatureClass(int fcIndex) const {
	char propName[64] = { '\0' };
	snprintf(propName, 64, "FC-%d", fcIndex);
	return propName;
}

void JobStructureModelGridding::CheckOrAddFieldIntoGrid(const geo3dml::Field& field) {
	wxCriticalSectionLocker locker(criticalSection_);
	if (!g3dVoxelGrid_->HasField(field.Name())) {
		g3dVoxelGrid_->AddField(field);
	}
}

void JobStructureModelGridding::JobThread::SubmitCells(const std::vector<g3dgrid::VoxelCell>& cells) {
	wxLogInfo(Strings::MessageOfGriddingProgress(GetId(), (int)cells.size()));
}

bool JobStructureModelGridding::JobThread::IntersectWithLine(vtkTriangle* triangle, double p1[3], double p2[3], double x[3]) const {
	double* bounds = triangle->GetBounds();
	if (p1[0] < bounds[0] || p1[0] > bounds[1] || p1[1] < bounds[2] || p1[1] > bounds[3]) {
		return false;
	}
	int subId;
	double t, pcoords[3];
	// Cases of returning 1:
	// (1) line p1-p2 intersects triangle on one edge.
	// (2) line p1-p2 intersects traingle on one vertex.
	// (3) line p1-p2 intersects traingle in the triangle.
	// Cases of returning 0:
	// (1) line p1-p2 lies in the plane of the traingle.
	// (2) line segment p1-p2 dose not touch the plane of the triangle.
	// (3) line p1-p2 intersects the plane of the triangle at a point outside of the triangle.
	return triangle->IntersectWithLine(p1, p2, ZERO_ERROR, t, x, pcoords, subId) == 1;
}

bool JobStructureModelGridding::JobThread::JudgeOnInsersection(double x, double y, double z, const std::vector<int>& triangleIndices, const TinBuf* tinBuf) const {
	size_t numOfTriangles = triangleIndices.size();
	if (numOfTriangles < 2) {
		return true;
	} else if (numOfTriangles == 2) {
		//if (IsSameTriangle(triangleIndices[0], triangleIndices[1], tinBuf)) {
		//	return true;
		//}
		// reject intersection at the vertex
		int v1 = tinBuf->triangles_[triangleIndices[0] * 3];
		int v2 = tinBuf->triangles_[triangleIndices[0] * 3 + 1];
		int v3 = tinBuf->triangles_[triangleIndices[0] * 3 + 2];
		if (IsSamePosition(x, y, 0, tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], 0)) {
			return false;
		} else if (IsSamePosition(x, y, 0, tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], 0)) {
			return false;
		} else if (IsSamePosition(x, y, 0, tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], 0)) {
			return false;
		}
		// reject intersection if the two triangles lying in the same side of the share edge.
		int v4 = tinBuf->triangles_[triangleIndices[1] * 3];
		int v5 = tinBuf->triangles_[triangleIndices[1] * 3 + 1];
		int v6 = tinBuf->triangles_[triangleIndices[1] * 3 + 2];
		double shareVertex1[2], shareVertex2[2], vertex1[2], vertex2[2];
		if (v1 == v4) {
			shareVertex1[0] = tinBuf->vertices_[v1 * 3];
			shareVertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
			if (v2 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v2 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v3 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v3 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else {
				return false;
			}
		} else if (v1 == v5) {
			shareVertex1[0] = tinBuf->vertices_[v1 * 3];
			shareVertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
			if (v2 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v2 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v3 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v3 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else if (v1 == v6) {
			shareVertex1[0] = tinBuf->vertices_[v1 * 3];
			shareVertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
			if (v2 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v2 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v3 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v3 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else if (v2 == v4) {
			shareVertex1[0] = tinBuf->vertices_[v2 * 3];
			shareVertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
			if (v1 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v1 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v3 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v3 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else {
				return false;
			}
		} else if (v2 == v5) {
			shareVertex1[0] = tinBuf->vertices_[v2 * 3];
			shareVertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
			if (v1 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v1 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v3 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v3 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else if (v2 == v6) {
			shareVertex1[0] = tinBuf->vertices_[v2 * 3];
			shareVertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
			if (v1 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v1 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v3 * 3];
				vertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v3 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v3 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v3 * 3];
				shareVertex2[1] = tinBuf->vertices_[v3 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else if (v3 == v4) {
			shareVertex1[0] = tinBuf->vertices_[v3 * 3];
			shareVertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
			if (v1 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v1 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v2 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v2 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else {
				return false;
			}
		} else if (v3 == v5) {
			shareVertex1[0] = tinBuf->vertices_[v3 * 3];
			shareVertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
			if (v1 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v1 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v2 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v6 * 3];
				vertex2[1] = tinBuf->vertices_[v6 * 3 + 1];
			} else if (v2 == v6) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else if (v3 == v6) {
			shareVertex1[0] = tinBuf->vertices_[v3 * 3];
			shareVertex1[1] = tinBuf->vertices_[v3 * 3 + 1];
			if (v1 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v1 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v1 * 3];
				shareVertex2[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v2 * 3];
				vertex1[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else if (v2 == v4) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v5 * 3];
				vertex2[1] = tinBuf->vertices_[v5 * 3 + 1];
			} else if (v2 == v5) {
				shareVertex2[0] = tinBuf->vertices_[v2 * 3];
				shareVertex2[1] = tinBuf->vertices_[v2 * 3 + 1];
				vertex1[0] = tinBuf->vertices_[v1 * 3];
				vertex1[1] = tinBuf->vertices_[v1 * 3 + 1];
				vertex2[0] = tinBuf->vertices_[v4 * 3];
				vertex2[1] = tinBuf->vertices_[v4 * 3 + 1];
			} else {
				return false;
			}
		} else {
			return false;
		}
		double s1 = (vertex1[0] - shareVertex1[0]) * (shareVertex2[1] - shareVertex1[1]) - (vertex1[1] - shareVertex1[1]) * (shareVertex2[0] - shareVertex1[0]);
		double s2 = (vertex2[0] - shareVertex1[0]) * (shareVertex2[1] - shareVertex1[1]) - (vertex2[1] - shareVertex1[1]) * (shareVertex2[0] - shareVertex1[0]);
		return s1 * s2 < 0;
	} else {	// numOfTriangles > 2
		// the intersection must be one of the vertex.
		int v1 = tinBuf->triangles_[triangleIndices[0] * 3];
		int v2 = tinBuf->triangles_[triangleIndices[0] * 3 + 1];
		int v3 = tinBuf->triangles_[triangleIndices[0] * 3 + 2];
		int theShareVertex = 0;
		if (IsSamePosition(x, y, 0, tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], 0)) {
			theShareVertex = v1;
		} else if (IsSamePosition(x, y, 0, tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], 0)) {
			theShareVertex = v2;
		} else if (IsSamePosition(x, y, 0, tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], 0)) {
			theShareVertex = v3;
		} else {
			return false;
		}
		std::map<int, std::list<int>> nodeLinks;
		for (size_t triIndex = 0; triIndex < numOfTriangles; ++triIndex) {
			v1 = tinBuf->triangles_[triangleIndices[triIndex] * 3];
			v2 = tinBuf->triangles_[triangleIndices[triIndex] * 3 + 1];
			v3 = tinBuf->triangles_[triangleIndices[triIndex] * 3 + 2];
			if (theShareVertex == v1) {
				if (nodeLinks.find(v2) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v3);
					nodeLinks[v2] = nodes;
				} else {
					nodeLinks[v2].push_back(v3);
				}
				if (nodeLinks.find(v3) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v2);
					nodeLinks[v3] = nodes;
				} else {
					nodeLinks[v3].push_back(v2);
				}
			} else if (theShareVertex == v2) {
				if (nodeLinks.find(v1) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v3);
					nodeLinks[v1] = nodes;
				} else {
					nodeLinks[v1].push_back(v3);
				}
				if (nodeLinks.find(v3) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v1);
					nodeLinks[v3] = nodes;
				} else {
					nodeLinks[v3].push_back(v1);
				}
			} else {
				// theSharedVertex == v3
				if (nodeLinks.find(v1) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v2);
					nodeLinks[v1] = nodes;
				} else {
					nodeLinks[v1].push_back(v2);
				}
				if (nodeLinks.find(v2) == nodeLinks.cend()) {
					std::list<int> nodes;
					nodes.push_back(v1);
					nodeLinks[v2] = nodes;
				} else {
					nodeLinks[v2].push_back(v1);
				}
			}
		}
		std::list<int> polygon;
		std::map<int, std::list<int>>::iterator linkItor = nodeLinks.begin();
		polygon.push_back(linkItor->first);
		while (linkItor != nodeLinks.end()) {
			if (linkItor->second.size() > 0) {
				std::list<int>::const_iterator nodeItor = linkItor->second.cbegin();
				while (nodeItor != linkItor->second.cend()) {
					if (*nodeItor == linkItor->first) {
						linkItor->second.erase(nodeItor);
						break;
					} else {
						++nodeItor;
					}
				}
				if (linkItor->second.size() > 0) {
					polygon.push_back(linkItor->second.front());
					linkItor->second.pop_front();
					linkItor = nodeLinks.find(polygon.back());
				} else {
					break;
				}
			} else {
				break;
			}
		}
		if (polygon.size() < 3 || polygon.front() != polygon.back()) {
			// nodes can not connected to a closed polygon.
			return false;
		}
		int numOfPoints = polygon.size() - 1;
		polygon.pop_back();
		int nodeIndex = 0;
		double bounds[6] = { 0 };
		double* pts = new double[numOfPoints * 3];
		std::list<int>::const_iterator nodeItor = polygon.cbegin();
		pts[nodeIndex] = tinBuf->vertices_[*nodeItor * 3];
		pts[nodeIndex + 1] = tinBuf->vertices_[*nodeItor * 3 + 1];
		pts[nodeIndex + 2] = 0;
		bounds[0] = bounds[1] = pts[nodeIndex];
		bounds[2] = bounds[3] = pts[nodeIndex + 1];
		nodeIndex += 3;
		++nodeItor;
		while (nodeItor != polygon.cend()) {
			pts[nodeIndex] = tinBuf->vertices_[*nodeItor * 3];
			pts[nodeIndex + 1] = tinBuf->vertices_[*nodeItor * 3 + 1];
			pts[nodeIndex + 2] = 0;
			if (pts[nodeIndex] < bounds[0]) {
				bounds[0] = pts[nodeIndex];
			} else if (pts[nodeIndex] > bounds[1]) {
				bounds[1] = pts[nodeIndex];
			}
			if (pts[nodeIndex + 1] < bounds[2]) {
				bounds[2] = pts[nodeIndex + 1];
			} else if (pts[nodeIndex + 1] > bounds[3]) {
				bounds[3] = pts[nodeIndex + 1];
			}
			nodeIndex += 3;
			++nodeItor;
		}
		double pt[3] = { x, y, 0 }, n[3];
		int ret = vtkPolygon::PointInPolygon(pt, numOfPoints, pts, bounds, n);
		delete[] pts;
		return ret == 1;
	}
}

bool JobStructureModelGridding::JobThread::IsSamePosition(double x1, double y1, double z1, double x2, double y2, double z2, double tol, bool ignoreZ)  const {
	bool v = fabs(x1 - x2) < tol && fabs(y1 - y2) < tol;
	if (ignoreZ) {
		return v;
	} else {
		return v && fabs(z1 - z2) < tol;
	}
}

bool JobStructureModelGridding::JobThread::IsSameTriangle(int triangleIndex1, int triangleIndex2, const TinBuf* tinBuf) const {
	int v1 = tinBuf->triangles_[triangleIndex1 * 3];
	int v2 = tinBuf->triangles_[triangleIndex1 * 3 + 1];
	int v3 = tinBuf->triangles_[triangleIndex1 * 3 + 2];
	int v4 = tinBuf->triangles_[triangleIndex2 * 3];
	int v5 = tinBuf->triangles_[triangleIndex2 * 3 + 1];
	int v6 = tinBuf->triangles_[triangleIndex2 * 3 + 2];
	if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else if (IsSamePosition(tinBuf->vertices_[v3 * 3], tinBuf->vertices_[v3 * 3 + 1], tinBuf->vertices_[v3 * 3 + 2], tinBuf->vertices_[v6 * 3], tinBuf->vertices_[v6 * 3 + 1], tinBuf->vertices_[v6 * 3 + 2], ZERO_ERROR, false)) {
		if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else if (IsSamePosition(tinBuf->vertices_[v1 * 3], tinBuf->vertices_[v1 * 3 + 1], tinBuf->vertices_[v1 * 3 + 2], tinBuf->vertices_[v5 * 3], tinBuf->vertices_[v5 * 3 + 1], tinBuf->vertices_[v5 * 3 + 2], ZERO_ERROR, false)
			&& IsSamePosition(tinBuf->vertices_[v2 * 3], tinBuf->vertices_[v2 * 3 + 1], tinBuf->vertices_[v2 * 3 + 2], tinBuf->vertices_[v4 * 3], tinBuf->vertices_[v4 * 3 + 1], tinBuf->vertices_[v4 * 3 + 2], ZERO_ERROR, false)) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

std::map<void*, JobStructureModelGridding::TinBuf*> JobStructureModelGridding::tinBufs_;
std::shared_mutex JobStructureModelGridding::tinBufMutex_;

JobStructureModelGridding::TinBuf::TinBuf(int numOfVertices, int numOfTriangles) {
	numOfVertices_ = numOfVertices;
	numOfTriangles_ = numOfTriangles;
	if (numOfVertices_ > 0) {
		vertices_ = new double[numOfVertices_ * 3];
	} else {
		vertices_ = NULL;
	}
	if (numOfTriangles_ > 0) {
		triangles_ = new int[numOfTriangles_ * 3];
	} else {
		triangles_ = NULL;
	}
}
JobStructureModelGridding::TinBuf::~TinBuf() {
	if (vertices_ != NULL) {
		delete[] vertices_;
	}
	if (triangles_ != NULL) {
		delete[] triangles_;
	}
}

const JobStructureModelGridding::TinBuf* JobStructureModelGridding::GetTinBuf(geo3dml::TIN* tin) {
	TinBuf* tinBuf = NULL;
	tinBufMutex_.lock_shared();
	std::map<void*, TinBuf*>::const_iterator citor = tinBufs_.find(tin);
	if (citor != tinBufs_.cend()) {
		tinBuf = citor->second;
		tinBufMutex_.unlock_shared();
	} else {
		tinBufMutex_.unlock_shared();
		tinBufMutex_.lock();
		// check again
		citor = tinBufs_.find(tin);
		if (citor != tinBufs_.cend()) {
			tinBuf = citor->second;
		} else {
			int numOfVertices = tin->GetVertexCount();
			int numOfTriangles = tin->GetTriangleCount();
			tinBuf = new TinBuf(numOfVertices, numOfTriangles);
			double x = 0, y = 0, z = 0;
			for (int i = 0; i < numOfVertices; ++i) {
				tin->GetVertexAt(i, x, y, z);
				tinBuf->vertices_[i * 3] = x;
				tinBuf->vertices_[i * 3 + 1] = y;
				tinBuf->vertices_[i * 3 + 2] = z;
			}
			int v1 = 0, v2 = 0, v3 = 0;
			for (int i = 0; i < numOfTriangles; ++i) {
				tin->GetTriangleAt(i, v1, v2, v3);
				tinBuf->triangles_[i * 3] = v1;
				tinBuf->triangles_[i * 3 + 1] = v2;
				tinBuf->triangles_[i * 3 + 2] = v3;
			}
			tinBufs_[tin] = tinBuf;
		}
		tinBufMutex_.unlock();
	}
	return tinBuf;
}
