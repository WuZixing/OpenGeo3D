#include <g3dgrid/VoxelGrid.h>

using namespace g3dgrid;

VoxelGrid::VoxelGrid() {

}

VoxelGrid::~VoxelGrid() {
	for (auto itor = lods_.end(); itor != lods_.end(); ++itor) {
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

VoxelGrid& VoxelGrid::SetLOD(const LOD& lod) {
	int level = lod.GetLevel();
	auto itor = lods_.find(level);
	if (itor == lods_.cend()) {
		lods_[level] = new LOD(lod);
	} else {
		delete itor->second;
		itor->second = new LOD(lod);
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

bool VoxelGrid::GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) {
	return false;
}
