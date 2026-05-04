// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/AnchorPoint.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/API/GUI/Subcomponent.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
	struct TextCharacter {
		// Character position relative to the component's origin.
		vec2<float32> offset;

		vec2<float32> size;
		mat3x3<float32> textureTransform;
	};

	struct GUIContainer {
		uint32 parentIndex;
		svec2<float32> origin;
		Size2D size;

		/// The Z-index is relative to the parent's Z index.
		uint32 zIndex;

		svec2<float32> maximumSize;
		svec2<float32> minimumSize;
		svec2<float32> padding;

		// For non-text components, this is the background color blended with the image.
		// For text component, this is the text color.
		Color color = 0x00000000;

		// Exclusive to non-text components.
		AnchorPoint anchorPoint;
		LayoutDirection layoutDirection = LayoutDirection::HORIZONTAL;
		float32 layoutSpacing;
		ImageID image;

		// The type of the pointer to display when hovering over the component.
		PointerType pointerType;

		std::vector<uint32> childIndices;

		// Exclusive to text components.
		bool textComponent;
		sequence<char8> text;
		std::vector<TextCharacter> characters;
		FontID font;
		float32 fontSize;

		// TODO (debug-only)
		view<char8> debugName;
	};
}