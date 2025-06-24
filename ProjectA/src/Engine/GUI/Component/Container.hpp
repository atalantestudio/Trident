#pragma once

#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/FillMode.hpp"
#include "Engine/GUI/LayoutDirection.hpp"

namespace ProjectA {
	struct GUIContainer {
		uint32 parentIndex = ~0;

		// Extent2D origin;
		FillMode fillMode;
		Extent2D size;
		Extent2D padding;
		LayoutDirection layoutDirection;
		uint32 layoutSpacing;
		Color color = 0xffffffff;
	};
}