// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Lightmap {
		std::vector<vec2<float32>> coordinates;
		svec2<float32> offset;
		svec2<float32> size;
		float32 area;
	};
}