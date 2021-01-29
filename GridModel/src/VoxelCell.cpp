#include <g3dgrid/VoxelCell.h>

using namespace g3dgrid;

VoxelCell::VoxelCell(int i, int j, int k) : Voxel(i, j, k) {
	isValid_ = false;
}

VoxelCell::~VoxelCell() {
	auto citor = fieldValues_.cbegin();
	while (citor != fieldValues_.cend()) {
		delete citor->second;
		++citor;
	}
	fieldValues_.clear();
}

VoxelCell& VoxelCell::SetField(geo3dml::FieldValue* fv) {
	if (fv == nullptr) {
		return *this;
	}
	fieldValues_[fv->FieldName()] = fv;
	return *this;
}

geo3dml::FieldValue* VoxelCell::GetField(const std::string& name) const {
	if (fieldValues_.find(name) != fieldValues_.cend()) {
		return fieldValues_.find(name)->second;
	} else {
		return nullptr;
	}
}

std::vector<std::string> VoxelCell::GetFieldNames() const {
	std::vector<std::string> names;
	auto citor = fieldValues_.cbegin();
	while (citor != fieldValues_.cend()) {
		names.push_back(citor->first);
		++citor;
	}
	return names;
}

VoxelCell& VoxelCell::IsValid(bool v) {
	isValid_ = v;
	return *this;
}

bool VoxelCell::IsValid() const {
	return isValid_;
}
