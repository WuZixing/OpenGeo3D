#pragma once

#include <geo3dml/Geo3DML.h>
#include <geo3dml/Box3D.h>
#include <geo3dml/Point3D.h>
#include <geo3dml/UniformGrid.h>
#include "VoxelCell.h"

namespace g3dgrid {

	class LOD : public geo3dml::Object {
	public:
		LOD(int level, const geo3dml::Point3D& gridOrigin, const geo3dml::Point3D& voxelSize, int voxelScaleX, int voxelScaleY, int voxelScaleZ);
		LOD(const LOD& lod) = delete;
		virtual ~LOD();

		int GetLevel() const;
		geo3dml::Point3D GetGridOrigin() const;
		void GetVoxelSize(double& x, double& y, double& z) const;
		void GetVoxelScale(int& x, int& y, int& z) const;

		int GetVoxelCount() const;

		geo3dml::Box3D GetExtent() const;
		void Extend(const geo3dml::Box3D& range);

	private:
		int level_;
		int voxelScaleX_, voxelScaleY_, voxelScaleZ_;
		geo3dml::Point3D gridOrigin_, voxelSize_;
		geo3dml::UniformGrid* g3dUniformGrid_;
		Voxel voxelOffset_;
	};
}
