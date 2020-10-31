#pragma once

#include "VoxelGrid.h"

namespace g3dgrid {

	class GridCollection : public geo3dml::Object {
	public:
		GridCollection();
		virtual ~GridCollection();

		GridCollection& SetName(const std::string& name);
		std::string GetName();

		void AddGrid(Grid* g3dGrid);
		int GetGridCount();
		Grid* GetGridAt(int i);

		/// Compute the minimum bounding rectangle of grid models in this collection.
		/// @param minX Output the minimum X coordinate.
		/// @param minY Output the minimum Y coordinate.
		/// @param minZ Output the minimum X coordinate.
		/// @param maxX Output the maximum X coordinate.
		/// @param maxY Output the maximum Y coordinate.
		/// @param maxZ Output the maximum Z coordinate.
		/// @return In case of an empty collection, it will return false.
		bool GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ);

	private:
		std::string name_;
		std::vector<Grid*> grids_;
	};

}
