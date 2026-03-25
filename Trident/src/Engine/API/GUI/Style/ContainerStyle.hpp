// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/EventListeners.hpp"
#include "Engine/API/GUI/GUI.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
	struct ContainerStyle {
		// TODO(gui/component-id)
		// GUI::ComponentID id = max<GUI::ComponentID>();

		Size2D size;
		svec2<float32> maximumSize = {max<float32>(), max<float32>()};
		svec2<float32> minimumSize = {0_px, 0_px};
		uint32 zIndex = 0;
		svec2<float32> padding = {0_px, 0_px};
		AnchorPoint anchorPoint = AnchorPoint::TOP | AnchorPoint::LEFT;
		LayoutDirection layoutDirection = HORIZONTAL;
		float32 layoutSpacing = 0_px;
		Color color = 0x00000000;
		ImageID image = 0;
		PointerType pointerType = PointerType::DEFAULT;

		// TODO: If the ContainerStyle instance is not properly initialized, its event listeners will be null, thus crashing the GUI layer.
		const EventListeners* eventListeners = {};

		view<char8> debugName;
	};
}