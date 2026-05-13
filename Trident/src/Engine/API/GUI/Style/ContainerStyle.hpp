// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
	struct ContainerStyle {
		// TODO(gui/component-id)
		// GUI::ComponentID id = max<GUI::ComponentID>();

		/// Offset from the origin (only for floating components).
		svec2<float32> offset = {0_px, 0_px};

		/// The default sizing mode is FIT in both directions.
		Size2D size = {-2.0f, -2.0f};

		svec2<float32> maximumSize = {max<float32>(), max<float32>()};
		svec2<float32> minimumSize = {0_px, 0_px};
		uint32 zIndex = 0;

		/// Order: left, right, top, bottom.
		vec4<float32> padding = {0_px, 0_px, 0_px, 0_px};

		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		LayoutDirection layoutDirection = HORIZONTAL;

		/// Child offset along the layout axis (only for vertical lists).
		float32 layoutOffset = 0_px;

		float32 layoutSpacing = 0_px;
		Color color = 0x00000000;
		ImageID image = 0;
		PointerType pointerType = PointerType::INHERIT;
		view<char8> debugName;
	};
}