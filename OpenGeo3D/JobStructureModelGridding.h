#pragma once

#include "wxWidgets.h"
#include <shared_mutex>
#include <utility>
#include <vector>
#include <vtkPolyData.h>
#include <geo3dml/Box3D.h>
#include <geo3dml/FeatureClass.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/TIN.h>
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
	static void JobThreadQuit(wxThreadIdType threadId);
	static void Stop();

	JobStructureModelGridding() = delete;

private:
	struct TinBuf {
		TinBuf(int numOfVertices = 0, int numOfTriangles = 0);
		virtual ~TinBuf();
		int numOfVertices_, numOfTriangles_;
		int* triangles_;
		double* vertices_;
	};
	static const TinBuf* GetTinBuf(geo3dml::TIN* tin);
	static std::map<void*, TinBuf*> tinBufs_;
	static std::shared_mutex tinBufMutex_;

	class JobThread : public wxThread {
	public:
		// workerNo is zero-based index.
		JobThread(const FeatureClasses& featureClasses, const geo3dml::Box3D& range, g3dgrid::LOD* g3dGridLOD, wxCriticalSection* gridLODCS, int totalWokers, int wokerNo);
		virtual ~JobThread();

	protected:
		virtual ExitCode Entry() override;
		ExitCode RunGridding();

	private:
		// List sampling positions from bottomZ to topZ by half of stepZ.
		// Sampling positions on a pillar are labeled which feature they are in.
		vtkSmartPointer<vtkPolyData> NewPillar(double x, double y, double bottomZ, double topZ, double stepZ);
		std::string MakeFieldNameToFeatureClass(int fcIndex) const;
		void SubmitCells(const std::vector<g3dgrid::VoxelCell>& cells);

		// see vtkTriangle::IntersectWithLine
		bool IntersectWithLine(vtkTriangle* triangle, double p1[3], double p2[3], double x[3]) const;
		bool JudgeOnInsersection(double x, double y, double z, const std::vector<int>& triangleIndices, const TinBuf* tinBuf) const;
		bool IsSamePosition(double x1, double y1, double z1, double x2, double y2, double z2, double tol = ZERO_ERROR, bool ignoreZ = true) const;
		bool IsSameTriangle(int triangleIndex1, int triangleIndex2, const TinBuf* tinBuf) const;

	private:
		FeatureClasses sourceFeatureClasses_;
		geo3dml::Box3D range_;
		g3dgrid::LOD* g3dGridLOD_;
		wxCriticalSection* gridLODCS_;
		int totalWorkers_, workerNo_;

	private:
		geo3dml::MultiPoint* mPointAll_;
		geo3dml::MultiPoint* mPointNet_;
		geo3dml::MultiPoint* mPointSampling_;
	};
	
private:
	static void CheckOrAddFieldIntoGrid(const geo3dml::Field& field);

	static void Clear();

private:
	static g3dgrid::VoxelGrid* g3dVoxelGrid_;
	static int lodLevel_;
	static std::vector<JobThread*> jobThreads_;
	static wxCriticalSection criticalSection_;
};