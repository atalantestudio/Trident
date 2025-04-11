#pragma once

#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class Win32TestLayer : public Layer {
		public:
			void update() override {
				const Window& window = getWindow();
				const sint32* size = window.getSize();

				ENGINE_DEBUG("Size: X = [] Y = []", size[0], size[1]);
			}
	};
}