#pragma once

#include "Engine/GUI/AnchorPoint.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"
#include "Engine/GUI/Subcomponent.hpp"

namespace ProjectA {
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
		svec2<float32> maximumSize;
		svec2<float32> minimumSize;
		svec2<float32> padding;

		// For non-text components, this is the background color blended with the image.
		// For text component, this is the text color.
		Color color = 0x00000000;

		// Exclusive to non-text components.
		AnchorPoint anchorPoint;
		LayoutDirection layoutDirection;
		float32 layoutSpacing;
		ImageID image;
		float32 radius;
		std::vector<uint32> childIndices;
		bool sizeIncludesPaddingAndSpacing = false;

		// Exclusive to text components.
		bool textComponent;
		std::string text;
		std::vector<TextCharacter> characters;
		FontID font;
		float32 fontSize;

		// TODO: Debug-only.
		std::string_view debugName;
	};
}