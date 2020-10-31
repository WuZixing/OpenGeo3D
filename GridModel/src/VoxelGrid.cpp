#include <g3dgrid/VoxelGrid.h>

using namespace g3dgrid;

VoxelGrid::VoxelGrid() {

}

VoxelGrid::~VoxelGrid() {

}

VoxelGrid& VoxelGrid::SetName(const std::string& name) {
	name_ = name;
	return *this;
}

VoxelGrid& VoxelGrid::SetDescription(const std::string& description) {
	description_ = description;
	return *this;
}

VoxelGrid& VoxelGrid::SetSRS(const std::string& srs) {
	srs_ = srs;
	return *this;
}

VoxelGrid& VoxelGrid::SetOrigin(const geo3dml::Point3D& origin) {
	origin_ = origin;
	return *this;
}

std::string VoxelGrid::GetName()  const {
	return name_;
}

std::string VoxelGrid::GetDescription()  const {
	return description_;
}

std::string VoxelGrid::GetSRS()  const {
	return srs_;
}

geo3dml::Point3D VoxelGrid::GetOrigin()  const {
	return origin_;
}

int VoxelGrid::GetLodCount()  const {
	return 0;
}

Lod VoxelGrid::GetLodAt(int i) const {
	return Lod(-1);
}

bool VoxelGrid::GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) {
	return false;
}