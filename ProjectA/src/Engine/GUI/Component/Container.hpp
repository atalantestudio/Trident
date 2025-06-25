#pragma once

#include "Engine/GUI/Color.hpp"
#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct GUIContainer {
		uint32 parentIndex = ~0;
		svec2<float32> origin;
		Size2D size;
		svec2<float32> remainingSize;
		svec2<float32> padding;
		LayoutDirection layoutDirection;
		float32 layoutSpacing;
		Color color = 0xffffffff;
		ImageID image;
		float32 radius;
		std::vector<uint32> childIndices;
	};

	struct TextComponent {
		uint32 parentIndex = ~0;
		svec2<float32> origin;
		Size2D size;
		const char* text;
		uint32 characterCount;
		Color color = 0xffffffff;
		FontID font;
		float32 fontSize;
	};
}