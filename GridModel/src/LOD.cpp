#include <g3dgrid/LOD.h>
#include <g3dgrid/VoxelGrid.h>
#include <g3dvtk/ObjectFactory.h>

using namespace g3dgrid;

LOD::LOD(int level, const geo3dml::Point3D& gridOrigin, const geo3dml::Point3D& voxelSize, int voxelScaleX, int voxelScaleY, int voxelScaleZ) {
	level_ = level;
	gridOrigin_ = gridOrigin;
	voxelSize_ = voxelSize;
	voxelScaleX_ = voxelScaleX;
	voxelScaleY_ = voxelScaleY;
	voxelScaleZ_ = voxelScaleZ;
}

LOD::~LOD() {
}

int LOD::GetLevel() const {
	return level_;
}

geo3dml::Point3D LOD::GetGridOrigin() const {
	return gridOrigin_;
}

void LOD::GetVoxelSize(double& x, double& y, double& z) const {
	x = voxelSize_.x;
	y = voxelSize_.y;
	z = voxelSize_.z;
}

void LOD::GetVoxelScale(int& x, int& y, int& z) const {
	x = voxelScaleX_;
	y = voxelScaleY_;
	z = voxelScaleZ_;
}

int LOD::GetVoxelCount() const {
	return 0;
}

geo3dml::Box3D LOD::GetExtent() const {
	geo3dml::Box3D box;
	return box;
}

void LOD::Extend(const geo3dml::Box3D& range) {

}