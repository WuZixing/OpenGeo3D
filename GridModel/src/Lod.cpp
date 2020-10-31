#include <g3dgrid/Lod.h>

using namespace g3dgrid;

Lod::Lod(int level) : level_(level) {

}

Lod::~Lod() {

}

int Lod::GetLevel() const {
	return level_;
}
