#pragma once

#include "Engine/Extent.hpp"
#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/px.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct ContainerStyle {
		Size2D size;
		svec2<float32> padding = {0_px, 0_px};
		LayoutDirection layoutDirection = HORIZONTAL;
		float32 layoutSpacing = 0_px;
		Color color = 0xffffffff;
		float32 radius = 0.0f;
	};
}