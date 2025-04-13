#pragma once

#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class Win32TestLayer : public Layer {
		public:
			Win32TestLayer() {
				/*for (sint32 i = 0; i < 40; i += 1) {
					ENGINE_DEBUG("Scroll test []", i);
				}*/
			}

			void update(float64 fixedDeltaTime) override {
				const Window& window = getWindow();
				const bool minimized = window.isMinimized();

				//ENGINE_DEBUG("Width = []px", window.getSize()[1]);

				if (windowMinimized != minimized) {
					windowMinimized = minimized;

					//if (windowMinimized) ENGINE_DEBUG("Window has been minimized.");
					//if (!windowMinimized) ENGINE_DEBUG("Window has been restored to its normal size.");
				}
			}

		private:
			bool windowMinimized = false;
	};
}