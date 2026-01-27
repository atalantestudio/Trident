// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/EventListeners.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
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
		PointerType pointerType = PointerType::DEFAULT;

		// TODO(deprecated)
		float32 borderRadius = 0.0f;

		// TODO: If the ContainerStyle instance is not properly initialized, its event listeners will be null, thus crashing the GUI layer.
		const EventListeners* eventListeners = {};

		view<char8> debugName;
	};
}