#pragma once

#include <glfw/glfw3.h>

#include "Backend/OpenGL/OpenGLContext.hpp"

#include "Engine/Event/KeyEvent.hpp"
#include "Engine/Event/MouseEvent.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Context.hpp"
#include "Engine/Window.hpp"

namespace ProjectA {
	class Win32Window : public Window {
		public:
			Win32Window(const WindowDescriptor& descriptor);

			virtual ~Win32Window();

			virtual void* getNativeWindow() const {
				return window;
			}

			uint32 getWidth() const override {
				return data.width;
			}

			uint32 getHeight() const override {
				return data.height;
			}

			void setEventCallback(const EventCallback& callback) override {
				data.callback = callback;
			}

			void update() override;

		private:
			struct GLFWData {
				std::string title;

				// TODO: Use vec2<sint32>.
				sint32 width = 0;
				sint32 height = 0;

				EventCallback callback;
			};

		private:
			GLFWwindow* window;
			scope<Context> context;
			GLFWData data;
	};
}