#pragma once

#include "Engine/API/Color.hpp"
#include "Engine/GUI/ID.hpp"

namespace ProjectA {
	struct TextStyle {
		// Size2D size;
		Color color = 0xffffffff;
		FontID font;
		float32 fontSize;
	};
}