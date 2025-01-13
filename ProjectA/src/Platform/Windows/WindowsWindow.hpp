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
			GLFWwindow* window;
			Scope<Context> context;

			struct GLFWData {
				std::string title;
				unsigned int width = 0;
				unsigned int height = 0;
				bool vSync = false;
				EventCallback callback;
			};

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

			unsigned int getWidth() const override {
				return data.width;
			}

			unsigned int getHeight() const override {
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