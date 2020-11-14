#include "JobStructureModelGridding.h"
#include <wx/log.h>
#include "Events.h"
#include "Strings.h"

std::vector<JobStructureModelGridding::JobThread*> JobStructureModelGridding::jobThreads_;
wxCriticalSection JobStructureModelGridding::CriticalSection_;

void JobStructureModelGridding::Start(const FeatureClasses& featureClasses, const g3dgrid::VoxelGrid* g3dVoxelGrid, const g3dgrid::LOD* g3dGridLOD) {
	wxCriticalSectionLocker locker(CriticalSection_);
	if (!jobThreads_.empty()) {
		return;
	}
	if (featureClasses.empty()) {
		wxLogInfo(wxS("stop gridding since no feature class is provided."));
		return;
	}
	double minX = 0, minY = 0, minZ = 0, maxX = -1, maxY = -1, maxZ = -1;
	size_t n = 0;
	for (; n < featureClasses.size(); ++n) {
		if (featureClasses[n].first->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ)) {
			break;
		}
	}
	if (n >= featureClasses.size()) {
		wxLogWarning(wxS("stop gridding since it is failed to get the MBR(minimum bounding rectangle) of feature classes."));
		return;
	}
	for (++n; n < featureClasses.size(); ++n) {
		double mbr[6] = { 0 };
		if (featureClasses[n].first->GetMinimumBoundingRectangle(mbr[0], mbr[1], mbr[2], mbr[3], mbr[4], mbr[5])) {
			if (minX > mbr[0]) {
				minX = mbr[0];
			}
			if (minY > mbr[1]) {
				minY = mbr[1];
			}
			if (minZ > mbr[2]) {
				minZ = mbr[2];
			}
			if (maxX < mbr[3]) {
				maxX = mbr[3];
			}
			if (maxY < mbr[4]) {
				maxY = mbr[4];
			}
			if (maxZ < mbr[5]) {
				maxZ = mbr[5];
			}
		}
	}
}

bool JobStructureModelGridding::IsRunning() {
	wxCriticalSectionLocker locker(CriticalSection_);
	return !jobThreads_.empty();
}

void JobStructureModelGridding::Stop() {
	wxCriticalSectionLocker locker(CriticalSection_);
	for (JobThread* job : jobThreads_) {
		job->Delete();	// wxThread::Wait() dose not ask the thread to quit, but only wait for it to quit.
		delete job;
	}
	jobThreads_.clear();
}

void JobStructureModelGridding::JobThreadQuit(JobThread* thread) {
	wxCriticalSectionLocker locker(CriticalSection_);
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

JobStructureModelGridding::JobThread::JobThread() : wxThread(wxTHREAD_JOINABLE) {

}

JobStructureModelGridding::JobThread::~JobThread() {

}

JobStructureModelGridding::JobThread::ExitCode JobStructureModelGridding::JobThread::Entry() {
	return ExitCode(0);
}