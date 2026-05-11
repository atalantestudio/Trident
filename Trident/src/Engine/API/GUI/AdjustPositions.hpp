// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	void GUILayer::PositionsAndAlignment(GUIContainer& component, svec2<float32> origin, bool registerSubcomponents) {
		const svec2<float32> previousOrigin = origin;

		if (component.parentIndex != max<uint32>()) {
			const GUIContainer& parent = components[component.parentIndex];

			if (parent.type == GUIComponentType::VERTICAL_LIST) {
				origin.y -= parent.layoutOffset;
			}
		}

		component.origin = origin;

		if (component.floating) {
			component.origin += component.offset;
		}

		if (registerSubcomponents) {
			registerSubcomponents = registerSubcomponent(component);
		}

		if (component.childIndices.size() == 0) {
			const GUIContainer& parent = components[component.parentIndex];

			// Skip over the component towards the layout direction (this includes the padding).
			origin[parent.layoutDirection] += component.size[parent.layoutDirection].value;
		}
		else {
			// Add top/left padding.
			origin.x += component.padding.x;
			origin.y += component.padding.y;

			svec2<float32> remainingSize = {component.size.width.value - component.padding.x * 2.0f, component.size.height.value - component.padding.y * 2.0f};
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
				GUIContainer& child = components[component.childIndices[index]];

				if (!child.floating) {
					const float32 remainingAcross = componentAcross - child.size[!component.layoutDirection].value;

					origin[!component.layoutDirection] = originAcross + remainingAcross * fAcross;
				}

				PositionsAndAlignment(child, origin, registerSubcomponents);

				if (!child.floating) {
					origin[component.layoutDirection] += child.size[component.layoutDirection].value;
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

	bool GUILayer::registerSubcomponent(const GUIContainer& component) {
		if (component.debugName == "__root") {
			return true;
		}

		const GUIContainer& parent = components[component.parentIndex];

		if (component.textComponent) {
			GUIGlyphSubcomponent glyphSubcomponent{};
			glyphSubcomponent.color = component.color.value;
			glyphSubcomponent.zIndex = component.zIndex;
			glyphSubcomponent.textureArrayLayerIndex = fonts[component.font].glyphMapImageId;
			glyphSubcomponent.fontSize = component.fontSize;

			// TODO(gui/text-smoothness)
			// glyphSubcomponent.a = a;
			// glyphSubcomponent.b = b;

			const vec2<float32> origin = {component.origin.x + 0.5f, component.origin.y + 0.5f};

			for (const TextCharacter& textCharacter : component.characters) {
				glyphSubcomponent.world =
					mat3x3<float32>::scaling(textCharacter.size) *
					mat3x3<float32>::translation(origin + textCharacter.offset);
				glyphSubcomponent.textureWorld =
					mat3x3<float32>::scaling((textCharacter.endTextureCoordinate - textCharacter.startTextureCoordinate)) *
					mat3x3<float32>::translation(textCharacter.startTextureCoordinate);

				renderer.registerGlyphSubcomponent(glyphSubcomponent);
			}

			return false;
		}

		float32 x = component.origin.x;
		float32 y = component.origin.y;

		float32 width = component.size.width.value;
		float32 height = component.size.height.value;

		if (parent.type == GUIComponentType::VERTICAL_LIST) {
			if (component.origin.y + component.size.height.value <= parent.origin.y) {
				return false;
			}

			if (component.origin.y >= parent.origin.y + parent.size.height.value) {
				return false;
			}

			const float32 clippedHeight = (parent.origin.y + parent.size.height.value) - component.origin.y;

			if (clippedHeight < height) {
				height = clippedHeight;
			}

			// TODO(gui/scroll-container/clipping): Finish clipping.
		}

		const vec2<float32> decimalOrigin = {x, round(y)};
		const vec2<float32> decimalSize = {width, height};
		const Extent2D imageExtent = renderer.getTextureArrayLayerExtent(component.image);
		const vec2<float32> imageSize = {static_cast<float32>(imageExtent.width), static_cast<float32>(imageExtent.height)};

		GUISubcomponent subcomponent{};
		subcomponent.worldTransform = mat3x3<float32>::scaling(decimalSize) * mat3x3<float32>::translation(decimalOrigin);
		subcomponent.textureTransform = mat3x3<float32>::scaling(imageSize / renderer.TEXTURE_ARRAY_MAX_SIZE);
		subcomponent.color = component.color.value;
		subcomponent.zIndex = component.zIndex;
		subcomponent.textureArrayLayerIndex = component.image;

		renderer.registerSubcomponent(subcomponent);

		return true;
	}
}