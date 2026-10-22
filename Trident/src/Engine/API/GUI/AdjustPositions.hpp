// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::PositionsAndAlignment(GUIContainer& component, uint32 componentIndex, vec2<float32> origin, bool registerSubcomponents) {
		if (component.parentIndex != max<uint32>()) {
			const GUIContainer& parent = components[component.parentIndex];

			if (parent.type == GUIComponentType::VERTICAL_LIST && parent.layoutOffset != nullptr) {
				origin.y -= *(parent.layoutOffset);
			}
		}

		component.origin = origin;

		if (component.floating) {
			component.origin += component.offset;
		}

		if (registerSubcomponents) {
			registerSubcomponents = registerComponent(component, componentIndex);
		}

		if (component.childIndices.size() == 0) {
			const GUIContainer& parent = components[component.parentIndex];

			// Skip over the component towards the layout direction (this includes the padding).
			origin[parent.layoutDirection] += component.size[parent.layoutDirection].value;
		}
		else {
			const float32 horizontalPadding = component.padding[0] + component.padding[1];
			const float32 verticalPadding = component.padding[2] + component.padding[3];

			// Add top/left padding.
			origin.x += component.padding[0];
			origin.y += component.padding[2];

			vec2<float32> remainingSize = {component.size.width.value - horizontalPadding, component.size.height.value - verticalPadding};
			float32 remainingAcross = 0.0f;

			for (const uint32 childIndex : component.childIndices) {
				const GUIContainer& child = components[childIndex];

				remainingSize[component.layoutDirection] -= child.size[component.layoutDirection].value;
				remainingAcross = max(remainingAcross, child.size[!component.layoutDirection].value);
			}

			remainingSize[component.layoutDirection] -= component.layoutSpacing * (static_cast<uint32>(component.childIndices.size()) - 1);
			remainingSize[!component.layoutDirection] -= remainingAcross;

			// Snap to anchor point along layout direction.
			const uint8 shiftAmountAlong = 4 - component.layoutDirection * 3;
			const uint8 maskAlong = 0b111000 >> component.layoutDirection * 3;
			const uint8 anchorRangeAlong = (static_cast<uint8>(component.anchorPoint) & maskAlong) >> shiftAmountAlong;
			const float32 fAlong = static_cast<float32>(anchorRangeAlong) * 0.5f;

			origin[component.layoutDirection] += remainingSize[component.layoutDirection] * fAlong;

			// Snap to anchor point across layout direction.
			const uint8 shiftAmountAcross = 1 + component.layoutDirection * 3;
			const uint8 maskAcross = 0b111000 >> (!component.layoutDirection) * 3;
			const uint8 anchorRangeAcross = (static_cast<uint8>(component.anchorPoint) & maskAcross) >> shiftAmountAcross;
			const float32 fAcross = static_cast<float32>(anchorRangeAcross) * 0.5f;
			const float32 componentAcross = component.size[!component.layoutDirection].value - component.padding[!component.layoutDirection] * 2.0f;
			const float32 originAcross = origin[!component.layoutDirection];

			index index = 0;

			while (index < component.childIndices.size()) {
				const uint32 childIndex = component.childIndices[index];

				GUIContainer& child = components[childIndex];

				if (!child.floating) {
					const float32 remainingAcross = componentAcross - round(child.size[!component.layoutDirection].value);

					origin[!component.layoutDirection] = originAcross + remainingAcross * fAcross;
				}

				PositionsAndAlignment(child, childIndex, origin, registerSubcomponents);

				if (!child.floating) {
					origin[component.layoutDirection] += round(child.size[component.layoutDirection].value);
				}

				if (index + 1 == component.childIndices.size()) {
					break;
				}

				// Add layout spacing (only if it's not the last child).
				origin[component.layoutDirection] += component.layoutSpacing;

				index += 1;
			}

			// Add padding along the layout direction.
			origin[component.layoutDirection] += component.padding[component.layoutDirection];

			// Remove padding across the layout direction.
			origin[!component.layoutDirection] -= component.padding[!component.layoutDirection];
		}
	}
}