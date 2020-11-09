#include <g3dgrid/Lod.h>

using namespace g3dgrid;

LOD::LOD(int level) : level_(level) {
	cellScaleX_ = cellScaleY_ = cellScaleZ_ = 1;
	cellSizeX_ = 0, cellSizeY_ = 0, cellSizeZ_ = 0;
}

LOD::LOD(const LOD& lod) {
	SetID(((LOD*)(&lod))->GetID());
	cellScaleX_ = lod.cellScaleX_;
	cellScaleY_ = lod.cellScaleY_;
	cellScaleZ_ = lod.cellScaleZ_;
	cellSizeX_ = lod.cellSizeX_;
	cellSizeY_ = lod.cellSizeY_;
	cellSizeZ_ = lod.cellSizeZ_;
}

LOD::~LOD() {

}

LOD& LOD::operator=(const LOD& lod) {
	if (this == &lod) {
		return *this;
	}
	SetID(((LOD*)(&lod))->GetID());
	cellScaleX_ = lod.cellScaleX_;
	cellScaleY_ = lod.cellScaleY_;
	cellScaleZ_ = lod.cellScaleZ_;
	cellSizeX_ = lod.cellSizeX_;
	cellSizeY_ = lod.cellSizeY_;
	cellSizeZ_ = lod.cellSizeZ_;
	return *this;
}

int LOD::GetLevel() const {
	return level_;
}

LOD& LOD::SetCellSize(double x, double y, double z) {
	cellSizeX_ = x;
	cellSizeY_ = y;
	cellSizeZ_ = z;
	return *this;
}

LOD& LOD::SetCellScale(int x, int y, int z) {
	cellScaleX_ = x;
	cellScaleY_ = y;
	cellScaleZ_ = z;
	return *this;
}

void LOD::GetCellSize(double& x, double& y, double& z) const {
	x = cellSizeX_;
	y = cellSizeY_;
	z = cellSizeZ_;
}

void LOD::GetCellScale(int& x, int& y, int& z) const {
	x = cellScaleX_;
	y = cellScaleY_;
	z = cellScaleZ_;
}
