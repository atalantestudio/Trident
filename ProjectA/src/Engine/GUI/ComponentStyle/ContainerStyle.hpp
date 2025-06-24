#pragma once

#include "Engine/Extent.hpp"
#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/FillMode.hpp"
#include "Engine/GUI/LayoutDirection.hpp"

namespace ProjectA {
	struct ContainerStyle {
		FillMode fillMode = FIXED;
		Extent2D size = {0, 0};
		Extent2D padding = {0, 0};
		LayoutDirection layoutDirection = HORIZONTAL;
		uint32 layoutSpacing = 0;
		Color color = 0xffffffff;
	};
}