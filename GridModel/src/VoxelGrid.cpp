#include <g3dgrid/VoxelGrid.h>

using namespace g3dgrid;

Voxel VoxelGrid::PointToVoxel(const geo3dml::Point3D& origin, const geo3dml::Point3D& step, const geo3dml::Point3D& point) {
	int i = (int)floor((point.x - origin.x) / step.x + 1e-6);
	int j = (int)floor((point.y - origin.y) / step.y + 1e-6);
	int k = (int)floor((point.z - origin.z) / step.z + 1e-6);
	return Voxel(i, j, k);
}

geo3dml::Point3D VoxelGrid::VoxelAnchor(const geo3dml::Point3D& origin, const geo3dml::Point3D& step, const Voxel& voxel) {
	double x = voxel.i * step.x + origin.x;
	double y = voxel.j * step.y + origin.y;
	double z = voxel.k * step.z + origin.z;
	return geo3dml::Point3D(x, y, z);
}

VoxelBox VoxelGrid::BoxToVoxelBox(const geo3dml::Point3D& origin, const geo3dml::Point3D& step, const geo3dml::Box3D& box) {
	Voxel min, max;
	min.i = (int)floor((box.min.x - origin.x) / step.x + 1e-6);
	min.j = (int)floor((box.min.y - origin.y) / step.y + 1e-6);
	min.k = (int)floor((box.min.z - origin.z) / step.z + 1e-6);
	max.i = (int)floor((box.max.x - origin.x) / step.x - 1e-6);
	max.j = (int)floor((box.max.y - origin.y) / step.y - 1e-6);
	max.k = (int)floor((box.max.z - origin.z) / step.z - 1e-6);
	return VoxelBox(min, max);
}

VoxelGrid::VoxelGrid() {

}

VoxelGrid::~VoxelGrid() {
	for (auto itor = lods_.begin(); itor != lods_.end(); ++itor) {
		delete itor->second;
	}
	lods_.clear();
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

VoxelGrid& VoxelGrid::SetLOD(LOD* lod) {
	int level = lod->GetLevel();
	auto itor = lods_.find(level);
	if (itor == lods_.cend()) {
		lods_[level] = lod;
	} else {
		delete itor->second;
		itor->second = lod;
	}
	return *this;
}

VoxelGrid& VoxelGrid::DeleteLOD(int level) {
	auto itor = lods_.find(level);
	if (itor != lods_.end()) {
		delete itor->second;
		lods_.erase(itor);
	}
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

int VoxelGrid::GetLODCount()  const {
	return static_cast<int>(lods_.size());
}

int VoxelGrid::GetMaxLOD() const {
	auto itor = lods_.cbegin();
	if (itor == lods_.cend()) {
		return -1;
	} else {
		int maxLevel = itor->first;
		++itor;
		while (itor != lods_.cend()) {
			if (maxLevel < itor->first) {
				maxLevel = itor->first;
			}
			++itor;
		}
		return maxLevel;
	}
}

LOD* VoxelGrid::GetLOD(int level) const {
	auto itor = lods_.find(level);
	if (itor != lods_.cend()) {
		return itor->second;
	} else {
		return nullptr;
	}
}

bool VoxelGrid::GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) const {
	return false;
}

VoxelGrid& VoxelGrid::AddField(const geo3dml::Field& f) {
	fields_.push_back(f);
	return *this;
}

int VoxelGrid::GetFieldCount() const {
	return (int)fields_.size();
}

const geo3dml::Field& VoxelGrid::GetFieldAt(int i) const {
	return fields_.at(i);
}

bool VoxelGrid::HasField(const std::string& name) const {
	for (auto citor = fields_.cbegin(); citor != fields_.cend(); ++citor) {
		if (name == citor->Name()) {
			return true;
		}
	}
	return false;
}
