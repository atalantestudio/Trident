// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Size.hpp"

namespace Trident {
	struct VerticalListStyle {
		/// The default sizing mode is FIT in both directions.
		Size2D size = {-2.0f, -2.0f};

		vec2<float32> maximumSize = {max<float32>(), max<float32>()};
		vec2<float32> minimumSize = {0_px, 0_px};
		uint32 zIndex = 0;

		/// Order: left, right, top, bottom.
		vec4<float32> padding = {0_px, 0_px, 0_px, 0_px};

		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;

		/// Child offset along the layout axis.
		float32* scrollOffset = nullptr;

		float32 layoutSpacing = 0_px;

		Color color = 0x00000000;
		ImageID image = 0;

		view<char8> debugName;
	};
}