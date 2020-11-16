#pragma once

#include "wxWidgets.h"
#include <utility>
#include <vector>
#include <geo3dml/Box3D.h>
#include <geo3dml/FeatureClass.h>
#include <g3dgrid/VoxelGrid.h>
#include <g3dvtk/UniformGrid.h>

class JobStructureModelGridding {
public:
	enum class FeatureType {
		Unknown,
		GeologicalBody,
		GeologicalSamplePoint
	};
	static wxString NameOfFeatureType(FeatureType type);
	static FeatureType NameToFeatureType(const wxString& name);

	using FeatureClassItem = std::pair<geo3dml::FeatureClass*, FeatureType>;
	using FeatureClasses = std::vector<FeatureClassItem>;
	
	static void Start(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::VoxelGrid* g3dVoxelGrid, int lodLevel);
	static bool IsRunning();
	static void Stop();

	JobStructureModelGridding() = delete;

private:
	class JobThread : public wxThread {
	public:
		JobThread(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::LOD* g3dGridLOD, wxCriticalSection* gridLODCS, int totalWokers, int wokerNo);
		virtual ~JobThread();

	protected:
		virtual ExitCode Entry() override;

	private:
		FeatureClasses sourceFeatureClasses_;
		geo3dml::Box3D range_;
		g3dgrid::LOD* g3dGridLOD_;
		wxCriticalSection* gridLODCS_;
		int totalWorkers_, workerNo_;
	};

protected:
	static void JobThreadQuit(JobThread* thread);

private:
	static g3dgrid::VoxelGrid* g3dVoxelGrid_;
	static int lodLevel_;
	static std::vector<JobThread*> jobThreads_;
	static wxCriticalSection criticalSection_;
};