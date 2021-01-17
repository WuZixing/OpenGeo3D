#pragma once

#include "Voxel.h"
#include <map>
#include <vector>
#include <geo3dml/FieldValue.h>

namespace g3dgrid {

	class VoxelCell : public Voxel {
	public:
		VoxelCell(int i, int j, int k);
		virtual ~VoxelCell();

		VoxelCell& SetField(geo3dml::FieldValue* fv);
		geo3dml::FieldValue* GetField(const std::string& name) const;
		std::vector<std::string> GetFieldNames() const;

	private:
		std::map<std::string, geo3dml::FieldValue*> fieldValues_;
	};

}