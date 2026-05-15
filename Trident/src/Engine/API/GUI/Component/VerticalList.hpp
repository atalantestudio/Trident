// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::openVerticalList(const ContainerStyle& style) {
		ENGINE_ASSERT(style.size.height.fillMode != FillMode::FIT, "FillMode::FIT cannot be used on the height of a vertical list.");
		ENGINE_ASSERT(style.layoutDirection != LayoutDirection::HORIZONTAL, "Vertical lists must have a vertical layout direction.");

		const uint32 parentIndex = openContainerIndices[openContainerIndexCount - 1];
		const GUIContainer& parent = components[parentIndex];

		GUIContainer component{};
		component.type = GUIComponentType::VERTICAL_LIST;
		component.parentIndex = parentIndex;
		component.floating = false;
		component.origin = parent.origin;
		component.offset = style.offset;
		component.size = style.size;
		component.zIndex = parent.zIndex + style.zIndex;

		setComponentSize(component, style.minimumSize, style.maximumSize);

		component.padding = style.padding;
		component.anchorPoint = style.anchorPoint;
		component.layoutDirection = style.layoutDirection;
		component.layoutOffset = style.layoutOffset;
		component.layoutSpacing = style.layoutSpacing;
		component.color = style.color;
		component.image = style.image;

		if (style.pointerType == PointerType::INHERIT) {
			component.pointerType = parent.pointerType;
		}
		else {
			component.pointerType = style.pointerType;
		}

		component.textComponent = false;
		component.debugName = style.debugName;

		// TODO(gui/component/vertical-list): Make focusable?
		openComponent(component, false);
	}
}