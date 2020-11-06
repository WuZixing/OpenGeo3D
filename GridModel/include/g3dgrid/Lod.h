#pragma once

namespace g3dgrid {

	class LOD {
	public:
		LOD(int level);
		virtual ~LOD();

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
