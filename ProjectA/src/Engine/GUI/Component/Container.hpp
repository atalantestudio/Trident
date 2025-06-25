#pragma once

#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct GUIContainer {
		uint32 parentIndex = ~0;

		svec2<float32> origin;

		// Child origin.
		svec2<float32> childOrigin;

		Size2D size;
		svec2<float32> remainingSize;
		svec2<float32> padding;
		LayoutDirection layoutDirection;
		float32 layoutSpacing;
		Color color = 0xffffffff;
		float32 radius;
		std::vector<uint32> childIndices;
	};
}