#pragma once

namespace g3dgrid {

	class Lod {
	public:
		Lod(int level);
		virtual ~Lod();

		int GetLevel() const;

	private:
		int level_;
	};
}
