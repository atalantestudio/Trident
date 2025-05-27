#pragma once

namespace ProjectA {
	struct TextStyle {
		vec4<float32> color = {0.0f, 0.0f, 0.0f, 1.0f};

		float32 fontSize = 64.0f;

		float32 outlineWidth = 0.0f;
		vec4<float32> outlineColor = {0.0f, 0.0f, 0.0f, 1.0f};
	};
}