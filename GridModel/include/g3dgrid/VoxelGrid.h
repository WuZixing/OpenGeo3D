#pragma once

#include <map>
#include <geo3dml/Geo3DML.h>
#include <geo3dml/Point.h>
#include "LOD.h"

namespace g3dgrid {

	class Grid : public geo3dml::Object {
	public:
		/// Compute the minimum bounding rectangle of the grid.
		/// @param minX Output the minimum X coordinate.
		/// @param minY Output the minimum Y coordinate.
		/// @param minZ Output the minimum X coordinate.
		/// @param maxX Output the maximum X coordinate.
		/// @param maxY Output the maximum Y coordinate.
		/// @param maxZ Output the maximum Z coordinate.
		/// @return In case of an empty grid, it will return false.
		virtual bool GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) = 0;
	};

	class VoxelGrid : public Grid {
	public:
		VoxelGrid();
		virtual ~VoxelGrid();

		VoxelGrid& SetName(const std::string& name);
		VoxelGrid& SetDescription(const std::string& description);
		VoxelGrid& SetSRS(const std::string& srs);
		VoxelGrid& SetOrigin(const geo3dml::Point3D& origin);
		VoxelGrid& SetLOD(const LOD& lod);
		VoxelGrid& DeleteLOD(int level);

		std::string GetName() const;
		std::string GetDescription() const;
		std::string GetSRS() const;
		geo3dml::Point3D GetOrigin() const;

		int GetLODCount() const;
		int GetMaxLOD() const;
		LOD* GetLOD(int level) const;

		virtual bool GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) override;

	private:
		std::string name_, description_, srs_;
		geo3dml::Point3D origin_;
		std::map<int, LOD*> lods_;
	};
}
