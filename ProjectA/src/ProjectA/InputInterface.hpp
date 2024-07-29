#pragma once

#include "core.hpp"

namespace ProjectA {
	class API InputInterface {
		private:
			static InputInterface* instance;

		protected:
			virtual bool isKeyPressedImpl(int keyCode) = 0;
			virtual bool isMouseButtonPressedImpl(int button) = 0;
			virtual std::pair<float, float> getMousePositionImpl() = 0;

		public:
			static inline bool isKeyPressed(int keyCode) {
				return instance->isKeyPressedImpl(keyCode);
			}

			static inline bool isMouseButtonPressed(int button) {
				return instance->isMouseButtonPressedImpl(button);
			}

			static inline std::pair<float, float> getMousePosition() {
				return instance->getMousePositionImpl();
			}
	};
}