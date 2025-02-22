#pragma once

#include <GLFW/glfw3.h>

#include "Platform/OpenGL/OpenGLContext.hpp"

#include "ProjectA/Event/KeyEvent.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "ProjectA/Renderer/Context.hpp"
#include "ProjectA/Window.hpp"

namespace ProjectA {
	class WindowsWindow : public Window {
		private:
			struct GLFWData {
				std::string title;
				uint32 width = 0;
				uint32 height = 0;
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

		public:
			WindowsWindow(const WindowData& data);

			virtual ~WindowsWindow();

			virtual void* getNativeWindow() const {
				return window;
			}

			// TODO: Merge width and height into vec2<int32> viewport
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
	};
}