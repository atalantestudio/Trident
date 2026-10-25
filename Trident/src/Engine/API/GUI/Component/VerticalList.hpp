// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::openVerticalList(const VerticalListStyle& style) {
		ENGINE_ASSERT(style.size.height.fillMode != FillMode::FIT, "FillMode::FIT cannot be used on the height of a vertical list.");

		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];
		const GUIComponent& parent = components[parentIndex];

		GUIComponent component{};
		component.type = GUIComponentType::VERTICAL_LIST;
		component.parentIndex = parentIndex;
		component.floating = false;
		component.origin = parent.origin;
		component.offset = {0_px, 0_px};
		component.size = style.size;
		component.zIndex = parent.zIndex + style.zIndex;

		setComponentSize(component, style.minimumSize, style.maximumSize);

		component.padding = style.padding;
		component.anchorPoint = style.anchorPoint;
		component.layoutDirection = LayoutDirection::VERTICAL;
		component.layoutOffset = style.scrollOffset;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;
		component.pointerType = parent.pointerType;

		#if defined(BUILD_MODE_DEBUG)
			component.debugName = style.debugName;
		#endif

		// TODO(gui/component/vertical-list): Make focusable?
		openComponent(component, false);
	}

	void GUILayer::updateVerticalList(MouseWheelEvent& event, GUIComponent& component) const {
		*component.layoutOffset -= event.getDelta() * 0.25f;

		// TODO(gui/component/vertical-list/clamp-scroll-offset)
		// const GUIComponent& lastChild = components[component.childIndices.back()];

		*component.layoutOffset = clamp(*component.layoutOffset, 0.0f, /* lastChild.origin.y + lastChild.size.height.value */ max<float32>());
	}
}