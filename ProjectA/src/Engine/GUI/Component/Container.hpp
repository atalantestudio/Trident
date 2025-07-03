#pragma once

#include "Engine/GUI/ID.hpp"
#include "Engine/GUI/LayoutDirection.hpp"
#include "Engine/GUI/Size.hpp"

namespace ProjectA {
	struct GUIContainer {
		uint32 parentIndex = ~0;
		svec2<float32> origin;
		Size2D size;
		svec2<float32> preferredSize;
		svec2<float32> minimumSize;
		svec2<float32> remainingSize;
		svec2<float32> padding;
		LayoutDirection layoutDirection;
		float32 layoutSpacing;
		Color color = 0xffffffff;
		ImageID image;
		float32 radius;
		std::vector<uint32> childIndices;

		// Exclusive to text components

		bool textComponent;
		const char* nullTerminatedText;
		FontID font;
		float32 fontSize;
	};
}