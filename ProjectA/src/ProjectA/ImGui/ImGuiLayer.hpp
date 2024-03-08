#pragma once

#include "ProjectA/Layer.hpp"
#include "ProjectA/Event/KeyEvent.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"

namespace ProjectA {
	class API ImGuiLayer : public Layer {
		private:
			float deltaTime = 0.0f;

			bool onKeyPress(KeyPressEvent& event);
			bool onKeyRelease(KeyReleaseEvent& event);
			bool onKeyType(KeyTypeEvent& event);
			bool onMouseButtonPress(MouseButtonPressEvent& event);
			bool onMouseButtonRelease(MouseButtonReleaseEvent& event);
			bool onMouseMove(MouseMoveEvent& event);
			bool onMouseScroll(MouseScrollEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		public:
			ImGuiLayer();
			~ImGuiLayer();

			void attach();
			void detach();
			void update();
			void listen(Event& event);
	};
}