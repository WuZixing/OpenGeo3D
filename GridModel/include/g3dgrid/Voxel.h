#pragma once

namespace g3dgrid {

	struct Voxel {
		Voxel(int i = 0, int j = 0, int k = 0) {
			this->i = i;
			this->j = j;
			this->k = k;
		}

		int i, j, k;
	};

	struct VoxelBox {
		VoxelBox() : min(0, 0, 0), max(-1, -1, -1) {};
		VoxelBox(const Voxel& min, const Voxel& max) {
			this->min = min;
			this->max = max;
		}

		bool IsEmpty() {
			return min.i > max.i;
		}
		
		Voxel min, max;
	};
}