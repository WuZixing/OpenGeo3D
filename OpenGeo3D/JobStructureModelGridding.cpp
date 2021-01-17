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
	jobThreads_.clear();
	g3dVoxelGrid_ = nullptr;
	lodLevel_ = -1;
}

void JobStructureModelGridding::JobThreadQuit(JobThread* thread) {
	wxCriticalSectionLocker locker(criticalSection_);
	auto itor = std::find(jobThreads_.cbegin(), jobThreads_.cend(), thread);
	if (itor == jobThreads_.cend()) {
		return;
	}
	wxThreadIdType threadId = thread->GetId();
	(*itor)->Delete();	// wxThread::Wait() dose not ask the thread to quit, but only wait for it to quit.
	delete (*itor);
	jobThreads_.erase(itor);
	if (jobThreads_.empty()) {
		wxLogInfo(Strings::MessageOfGriddingJobEnd());
	} else {
		wxLogInfo(Strings::MessageOfGriddingJobThreadStop(threadId, int(jobThreads_.size())));
	}
	Events::Notify(Events::ID::Notify_RefreshRenderWindow);
}

JobStructureModelGridding::JobThread::JobThread(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::LOD* g3dGridLOD, wxCriticalSection* gridLODCS, int totalWokers, int workerNo) : wxThread(wxTHREAD_JOINABLE) {
	sourceFeatureClasses_ = featureClasses;
	range_ = range;
	g3dGridLOD_ = g3dGridLOD;
	gridLODCS_ = gridLODCS;
	totalWorkers_ = totalWokers;
	workerNo_ = workerNo;
}

JobStructureModelGridding::JobThread::~JobThread() {

}

JobStructureModelGridding::JobThread::ExitCode JobStructureModelGridding::JobThread::Entry() {
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

				}
			}
		}
	}


	return ExitCode(0);
}

vtkSmartPointer<vtkPolyData> JobStructureModelGridding::JobThread::NewPillar(double x, double y, double bottomZ, double topZ, double stepZ) {
	return nullptr;
}

std::string JobStructureModelGridding::JobThread::MakeFieldNameToFeatureClass(int fcIndex) const {
	char propName[64] = { '\0' };
	snprintf(propName, 64, "FC-%d", fcIndex);
	return propName;
}