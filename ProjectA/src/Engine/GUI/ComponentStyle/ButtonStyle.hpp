#pragma once

#include "Engine/GUI/ComponentStyle/TextStyle.hpp"

namespace ProjectA {
	struct ButtonStyle {
		Extent2D padding;
		vec4<float32> backgroundColor;
		float32 radius;
		TextStyle* textStyle;
	};
}