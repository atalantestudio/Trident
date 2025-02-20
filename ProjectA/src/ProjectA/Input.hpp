#pragma once

#include "ProjectA/base.hpp"
#include "ProjectA/KeyCode.hpp"
#include "ProjectA/MouseButton.hpp"

namespace ProjectA {
	class Input {
		private:
			static Scope<Input> instance;

		protected:
			virtual bool isKeyPressedImpl(KeyCode keyCode) = 0;

			virtual bool isMouseButtonPressedImpl(MouseButton button) = 0;

			virtual lyah::vec2<float> getMousePositionImpl() = 0;

		public:
			static Scope<Input> create();

			static bool isKeyPressed(KeyCode keyCode) {
				return instance->isKeyPressedImpl(keyCode);
			}

			static bool isMouseButtonPressed(MouseButton button) {
				return instance->isMouseButtonPressedImpl(button);
			}

			static lyah::vec2<float> getMousePosition() {
				return instance->getMousePositionImpl();
			}
	};
}