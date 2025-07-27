#pragma once

#include "Engine/GUI/EventListeners.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct ContainerStyle {
		Size2D size;
		svec2<float32> maximumSize = {max<float32>(), max<float32>()};
		svec2<float32> minimumSize = {0_px, 0_px};
		svec2<float32> padding = {0_px, 0_px};
		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		LayoutDirection layoutDirection = HORIZONTAL;
		float32 layoutSpacing = 0_px;
		Color color = 0xffffffff;
		ImageID image = 0;
		float32 outlineSize = 0_px;
		Color outlineColor = 0x00000000;
		float32 borderRadius = 0.0f;
		const EventListeners* eventListeners = {};

		std::string_view debugName;
	};
}