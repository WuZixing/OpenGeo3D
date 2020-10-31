#include <g3dgrid/GridCollection.h>

using namespace g3dgrid;

GridCollection::GridCollection() {

}

GridCollection::~GridCollection() {
	for (Grid* grid : grids_) {
		delete grid;
	}
	grids_.clear();
}

GridCollection& GridCollection::SetName(const std::string& name) {
	name_ = name;
	return *this;
}

std::string GridCollection::GetName() {
	return name_;
}

void GridCollection::AddGrid(Grid* g3dGrid) {
	auto itor = std::find(grids_.cbegin(), grids_.cend(), g3dGrid);
	if (itor != grids_.cend()) {
		return;
	}
	grids_.push_back(g3dGrid);
}

int GridCollection::GetGridCount() {
	return (int)grids_.size();
}

Grid* GridCollection::GetGridAt(int i) {
	return grids_[i];
}

bool GridCollection::GetMinimumBoundingRectangle(double& minX, double& minY, double& minZ, double& maxX, double& maxY, double& maxZ) {
	size_t i = 0, numberOfGrids = grids_.size();
	for (; i < numberOfGrids; ++i) {
		if (grids_[i]->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ)) {
			break;
		}
	}
	if (i >= numberOfGrids) {
		return false;
	}
	double x[2], y[2], z[2];
	for (++i; i < numberOfGrids; ++i) {
		if (!grids_[i]->GetMinimumBoundingRectangle(x[0], y[0], z[0], x[1], y[1], z[1])) {
			continue;
		}
		if (x[0] < minX) {
			minX = x[0];
		}
		if (x[1] > maxX) {
			maxX = x[1];
		}
		if (y[0] < minY) {
			minY = y[0];
		}
		if (y[1] > maxY) {
			maxY = y[1];
		}
		if (z[0] < minZ) {
			minZ = z[0];
		}
		if (z[1] > maxZ) {
			maxZ = z[1];
		}
	}
	return true;
}