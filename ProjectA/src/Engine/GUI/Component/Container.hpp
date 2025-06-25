#pragma once

#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct GUIContainer {
		uint32 parentIndex = ~0;

		Extent2D origin;
		Size2D size;
		Extent2D padding;
		LayoutDirection layoutDirection;
		uint32 layoutSpacing;
		Color color = 0xffffffff;
		std::vector<uint32> childIndices;
	};
}