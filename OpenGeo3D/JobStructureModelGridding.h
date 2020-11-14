#pragma once

#include "wxWidgets.h"
#include <utility>
#include <vector>
#include <geo3dml/FeatureClass.h>
#include <g3dgrid/VoxelGrid.h>

class JobStructureModelGridding {
public:
	enum class FeatureType {
		Unknown,
		GeologicalBody,
		GeologicalSamplePoint
	};
	using FeatureClassItem = std::pair<geo3dml::FeatureClass*, FeatureType>;
	using FeatureClasses = std::vector<FeatureClassItem>;
	
	static void Start(const FeatureClasses& featureClasses, const g3dgrid::VoxelGrid* g3dVoxelGrid, const g3dgrid::LOD* g3dGridLOD);
	static bool IsRunning();
	static void Stop();

	JobStructureModelGridding() = delete;

private:
	class JobThread : public wxThread {
	public:
		JobThread();
		virtual ~JobThread();

	protected:
		virtual ExitCode Entry() override;
	};

protected:
	static void JobThreadQuit(JobThread* thread);

private:
	static std::vector<JobThread*> jobThreads_;
	static wxCriticalSection CriticalSection_;
};