#pragma once

#include <geo3dml/Geo3DML.h>

namespace g3dgrid {

	class LOD : public geo3dml::Object {
	public:
		LOD(int level);
		LOD(const LOD& lod);
		virtual ~LOD();
		LOD& operator=(const LOD& lod);

		LOD& SetCellSize(double x, double y, double z);
		LOD& SetCellScale(int x, int y, int z);

		int GetLevel() const;
		void GetCellSize(double& x, double& y, double& z) const;
		void GetCellScale(int& x, int& y, int& z) const;

	private:
		int level_;
		int cellScaleX_, cellScaleY_, cellScaleZ_;
		double cellSizeX_, cellSizeY_, cellSizeZ_;
	};
}
