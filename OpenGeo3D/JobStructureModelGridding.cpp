#include "JobStructureModelGridding.h"
#include <wx/log.h>
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
		JobThread* jobThread = new JobThread(featureClasses, range, nullptr, &criticalSection_, numberOfThreads, n);
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
	gridLODCS_->Leave();
	g3dgrid::Voxel minIJK = g3dgrid::VoxelGrid::PointToVoxel(origin, step, range_.min);
	g3dgrid::Voxel maxIJK = g3dgrid::VoxelGrid::PointToVoxel(origin, step, range_.max);

	return ExitCode(0);
}