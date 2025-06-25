#pragma once

#include "Engine/Extent.hpp"
#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/px.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct ContainerStyle {
		Size2D size;
		Extent2D padding = {0px, 0px};
		LayoutDirection layoutDirection = HORIZONTAL;
		uint32 layoutSpacing = 0px;
		Color color = 0xffffffff;
	};
}