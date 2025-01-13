#pragma once

#include "ProjectA/core.hpp"

namespace ProjectA {
	class InputInterface {
		private:
			static InputInterface* instance;

		protected:
			virtual bool isKeyPressedImpl(int keyCode) = 0;

			virtual bool isMouseButtonPressedImpl(int button) = 0;

			virtual vec2<float> getMousePositionImpl() = 0;

		public:
			static inline bool isKeyPressed(int keyCode) {
				return instance->isKeyPressedImpl(keyCode);
			}

			static inline bool isMouseButtonPressed(int button) {
				return instance->isMouseButtonPressedImpl(button);
			}

			static inline vec2<float> getMousePosition() {
				return instance->getMousePositionImpl();
			}
	};
}