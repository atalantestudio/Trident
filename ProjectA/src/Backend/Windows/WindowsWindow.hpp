#pragma once

#include <glfw/glfw3.h>

#include "Backend/OpenGL/OpenGLContext.hpp"

#include "Engine/Event/KeyEvent.hpp"
#include "Engine/Event/MouseEvent.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Renderer/Context.hpp"
#include "Engine/Window.hpp"

namespace ProjectA {
	class WindowsWindow : public Window {
		public:
			WindowsWindow(const WindowData& data);

			virtual ~WindowsWindow();

			virtual void* getNativeWindow() const {
				return window;
			}

			// TODO: Merge width and height into vec2<sint32> viewport
			uint32 getWidth() const override {
				return data.width;
			}

			uint32 getHeight() const override {
				return data.height;
			}

			bool isVSyncEnabled() const override;

			void setEventCallback(const EventCallback& callback) override {
				data.callback = callback;
			}

			void setVSyncState(bool state) override;

			void update() override;

		private:
			struct GLFWData {
				std::string title;
				// TODO: Use vec2<sint32>
				sint32 width = 0;
				sint32 height = 0;
				bool vSync = false;
				EventCallback callback;
			};

		private:
			GLFWwindow* window;
			scope<Context> context;
			GLFWData data;

		private:
			virtual void initialize(const WindowData& data);

			virtual void terminate();
	};
}