// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/AnchorPoint.hpp"
#include "Engine/API/GUI/ComponentType.hpp"
#include "Engine/API/GUI/ID.hpp"
#include "Engine/API/GUI/LayoutDirection.hpp"
#include "Engine/API/GUI/Size.hpp"
#include "Engine/API/GUI/Subcomponent.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
	struct TextCharacter {
		/// Character position relative to the component's origin.
		vec2<float32> offset;

		vec2<float32> size;
		vec2<float32> startTextureCoordinate;
		vec2<float32> endTextureCoordinate;
	};

	struct GUIContainer {
		GUIComponentType type;

		uint32 parentIndex;

		/// Floating components don't add to the size of their parent.
		bool floating;

		svec2<float32> origin;

		/// Offset to the origin (floating components only).
		svec2<float32> offset;

		Size2D size;

		/// The Z-index is relative to the parent's Z index.
		uint32 zIndex;

		svec2<float32> maximumSize;
		svec2<float32> minimumSize;

		/// Order: left, right, top, bottom.
		vec4<float32> padding;

		// For non-text components, this is the background color blended with the image.
		// For text component, this is the text color.
		Color color = 0x00000000;

		// Exclusive to non-text components.
		AnchorPoint anchorPoint;
		LayoutDirection layoutDirection;
		float32 layoutOffset;
		float32 layoutSpacing;
		ImageID image;

		// The type of the pointer to display when hovering over the component.
		PointerType pointerType;

		std::vector<uint32> childIndices;

		// Exclusive to text components.
		bool textComponent;

		// TODO(gui/component/text-input): Merge this and GUIContainer::text.
		std::string* inputText = nullptr;

		sequence<char8> text;
		std::vector<TextCharacter> characters;
		FontID font;
		float32 fontSize;

		// TODO (debug-only)
		view<char8> debugName;
	};
}