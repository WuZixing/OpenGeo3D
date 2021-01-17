#include <g3dgrid/LOD.h>
#include <g3dgrid/VoxelGrid.h>
#include <g3dvtk/ObjectFactory.h>

using namespace g3dgrid;

LOD::LOD(int level, const geo3dml::Point3D& gridOrigin, const geo3dml::Point3D& voxelSize, int voxelScaleX, int voxelScaleY, int voxelScaleZ) {
	level_ = level;
	gridOrigin_ = gridOrigin;
	voxelSize_ = voxelSize;
	voxelScaleX_ = voxelScaleX;
	voxelScaleY_ = voxelScaleY;
	voxelScaleZ_ = voxelScaleZ;
	g3dUniformGrid_ = nullptr;
}

LOD::~LOD() {
	if (g3dUniformGrid_ != nullptr) {
		delete g3dUniformGrid_;
	}
}

int LOD::GetLevel() const {
	return level_;
}

geo3dml::Point3D LOD::GetGridOrigin() const {
	return gridOrigin_;
}

void LOD::GetVoxelSize(double& x, double& y, double& z) const {
	x = voxelSize_.x;
	y = voxelSize_.y;
	z = voxelSize_.z;
}

void LOD::GetVoxelScale(int& x, int& y, int& z) const {
	x = voxelScaleX_;
	y = voxelScaleY_;
	z = voxelScaleZ_;
}

int LOD::GetVoxelCount() const {
	if (g3dUniformGrid_ == nullptr) {
		return 0;
	}
	int dimI = 0, dimJ = 0, dimK = 0;
	g3dUniformGrid_->GetDimensions(dimI, dimJ, dimK);
	return dimK * dimJ * dimI;
}

geo3dml::Box3D LOD::GetExtent() const {
	if (g3dUniformGrid_ == nullptr) {
		return geo3dml::Box3D();
	} else {
		geo3dml::Box3D box;
		g3dUniformGrid_->GetMinimumBoundingRectangle(box.min.x, box.min.y, box.min.z, box.max.x, box.max.y, box.max.z);
		return box;
	}
}

void LOD::Extend(const geo3dml::Box3D& range) {
	g3dvtk::ObjectFactory g3dFactory;
	VoxelBox newBox = VoxelGrid::BoxToVoxelBox(gridOrigin_, voxelSize_, range);

	geo3dml::Box3D oldRange = GetExtent();
	if (oldRange.IsEmpty()) {
		// g3dUniformGrid_ is null.
		int dimI = newBox.max.i - newBox.min.i + 1;
		int dimJ = newBox.max.j - newBox.min.j + 1;
		int dimK = newBox.max.k - newBox.min.k + 1;
		g3dUniformGrid_ = g3dFactory.NewUniformGrid(
			gridOrigin_.x, gridOrigin_.y, gridOrigin_.z,
			voxelSize_.x, voxelSize_.y, voxelSize_.z,
			dimI, dimJ, dimK);
		voxelOffset_ = newBox.min;
	} else {
		VoxelBox oldBox = VoxelGrid::BoxToVoxelBox(gridOrigin_, voxelSize_, oldRange);
		if (oldBox.min.i <= newBox.min.i && oldBox.min.j <= newBox.min.j && oldBox.min.k <= newBox.min.k
			&& oldBox.max.i >= newBox.max.i && oldBox.max.j >= newBox.max.j && oldBox.max.k >= newBox.max.k) {
			return;
		}
		if (newBox.min.i > oldBox.min.i) {
			newBox.min.i = oldBox.min.i;
		}
		if (newBox.min.j > oldBox.min.j) {
			newBox.min.j = oldBox.min.j;
		}
		if (newBox.min.k > oldBox.min.k) {
			newBox.min.k = oldBox.min.k;
		}
		if (newBox.max.i < oldBox.max.i) {
			newBox.max.i = oldBox.max.i;
		}
		if (newBox.max.j < oldBox.max.j) {
			newBox.max.j = oldBox.max.j;
		}
		if (newBox.max.k < oldBox.max.k) {
			newBox.max.k = oldBox.max.k;
		}
		int newDimI = newBox.max.i - newBox.min.i + 1;
		int newDimJ = newBox.max.j - newBox.min.j + 1;
		int newDimK = newBox.max.k - newBox.min.k + 1;
		geo3dml::UniformGrid* newUniformGrid = g3dFactory.NewUniformGrid(
			gridOrigin_.x, gridOrigin_.y, gridOrigin_.z,
			voxelSize_.x, voxelSize_.y, voxelSize_.z,
			newDimI, newDimJ, newDimK);
		newUniformGrid->SetName(g3dUniformGrid_->GetName());
		
		// shape property
		geo3dml::ShapeProperty* newVoxelProp = nullptr;
		geo3dml::ShapeProperty* oldVoxelProp = g3dUniformGrid_->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Voxel);
		if (oldVoxelProp != nullptr) {
			newVoxelProp = g3dFactory.NewShapeProperty();
			for (int n = 0; n < oldVoxelProp->GetFieldCount(); ++n) {
				newVoxelProp->AddField(oldVoxelProp->GetFieldAt(n));
			}
			newUniformGrid->SetProperty(newVoxelProp, geo3dml::ShapeProperty::SamplingTarget::Voxel);
		}

		// voxels
		int offsetI = oldBox.min.i - newBox.min.i;
		int offsetJ = oldBox.min.j - newBox.min.j;
		int offsetK = oldBox.min.k - newBox.min.k;
		int oldDimI = 0, oldDimJ = 0, oldDimK = 0;
		g3dUniformGrid_->GetDimensions(oldDimI, oldDimJ, oldDimK);
		for (int k = 0; k < oldDimK; ++k) {
			for (int j = 0; j < oldDimJ; ++j) {
				for (int i = 0; i < oldDimI; ++i) {
					newUniformGrid->SetCellValidation(i + offsetI, j + offsetJ, k + offsetK, g3dUniformGrid_->IsCellValid(i, j, k));
					
				}
			}
		}
		// voxel properties
		if (newVoxelProp != nullptr) {
			for (int k = 0; k < oldDimK; ++k) {
				for (int j = 0; j < oldDimJ; ++j) {
					for (int i = 0; i < oldDimI; ++i) {
						int oldVoxelIndex = k * oldDimJ * oldDimI + j * oldDimI + i;
						int newVoxelIndex = (k + offsetK) * newDimJ * newDimI + (j + offsetJ) * newDimI + (i + offsetI);
						for (int n = 0; n < newVoxelProp->GetFieldCount(); ++n) {
							geo3dml::Field field = newVoxelProp->GetFieldAt(n);
							switch (field.DataType()) {
							case geo3dml::Field::ValueType::Integer: {
								newVoxelProp->IntValue(n, newVoxelIndex, oldVoxelProp->IntValue(n, oldVoxelIndex));
								break;
							}
							case geo3dml::Field::ValueType::Double: {
								newVoxelProp->DoubleValue(n, newVoxelIndex, oldVoxelProp->DoubleValue(n, oldVoxelIndex));
								break;
							}
							case geo3dml::Field::ValueType::Text: {
								newVoxelProp->TextValue(n, newVoxelIndex, oldVoxelProp->TextValue(n, oldVoxelIndex));
								break;
							}
							case geo3dml::Field::ValueType::Boolean: {
								newVoxelProp->BooleanValue(n, newVoxelIndex, oldVoxelProp->BooleanValue(n, oldVoxelIndex));
								break;
							}
							default:
								break;
							}
						}
					}
				}
			}
		}
		delete g3dUniformGrid_;
		g3dUniformGrid_ = newUniformGrid;
		voxelOffset_ = newBox.min;
	}
}