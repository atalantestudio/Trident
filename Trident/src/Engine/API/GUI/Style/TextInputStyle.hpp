// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"

namespace Trident {
	struct TextInputStyle {
		/// The default sizing mode is FIT in both directions.
		Size2D size = {-2.0f, -2.0f};

		svec2<float32> maximumSize = {max<float32>(), max<float32>()};
		svec2<float32> minimumSize = {0_px, 0_px};
		uint32 zIndex = 0;

		/// Order: left, right, top, bottom.
		vec4<float32> padding = {0_px, 0_px, 0_px, 0_px};

		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		LayoutDirection layoutDirection = HORIZONTAL;

		float32 layoutSpacing = 0_px;

		Color color = 0x00000000;
		ImageID image = 0;

		std::string* text = nullptr;

		view<char8> debugName;
	};
}