#include "DemoGUILayer.hpp"

namespace ProjectA {
	DemoGUILayer::DemoGUILayer() {
		// Register colors.
		{
			const chain<vec4<float32>> colors = {
				{1.0f, 1.0f, 1.0f, 1.0f},
				{1.0f, 0.2f, 0.0f, 1.0f},
				{0.1f, 0.1f, 0.2f, 1.0f},
			};

			registerColors(colors);
		}

		append(ColoredQuad({0.0f, 0.0f}, {64.0f, 64.0f}, 0));
		append(ColoredQuad({16.0f, 80.0f}, {128.0f, 48.0f}, 1));
		append(ColoredQuad({80.0f, 128.0f}, {256.0f, 512.0f}, 2));

		registerEntityPool();
	}
}