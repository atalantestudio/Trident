// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
	struct TextInputStyle {
		/// The width of the text input.
		/// The height is automatically set to the line height of the font.
		Size1D width;

		vec2<float32> maximumSize = {max<float32>(), max<float32>()};
		vec2<float32> minimumSize = {0_px, 0_px};
		uint32 zIndex = 0;

		/// Order: left, right, top, bottom.
		vec4<float32> padding = {0_px, 0_px, 0_px, 0_px};

		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		LayoutDirection layoutDirection = HORIZONTAL;

		float32 layoutSpacing = 0_px;

		Color color = 0x00000000;
		ImageID image = 0;

		UTF8String* text = nullptr;
		FontID font;
		float32 fontSize;

		#if defined(BUILD_MODE_DEBUG)
			view<char8> debugName;
		#endif
	};
}