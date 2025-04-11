#pragma once

#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class Win32TestLayer : public Layer {
		public:
			void update() override {
				const Window& window = getWindow();
				const sint32* mousePosition = window.getMousePosition();

				ENGINE_DEBUG("Mouse: X = [] Y = []", mousePosition[0], mousePosition[1]);
			}
	};
}