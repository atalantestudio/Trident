#pragma once

#include "ProjectA/Renderer/Context.hpp"
#include "ProjectA/WindowInterface.hpp"
#include <GLFW/glfw3.h>

namespace ProjectA {
	class Window : public WindowInterface {
		private:
			GLFWwindow* window;
			Context* context;

			struct GLFWData {
				std::string title;
				unsigned int width = 0, height = 0;
				bool vSync = false;
				EventCallback callback;
			};

			GLFWData data;

		private:
			virtual void initialize(const WindowData& data);
			virtual void terminate();

		public:
			Window(const WindowData& data);
			virtual ~Window();

			virtual inline void* getNativeWindow() const {
				return window;
			}

			inline unsigned int getWidth() const override {
				return data.width;
			}

			inline unsigned int getHeight() const override {
				return data.height;
			}

			bool isVSyncEnabled() const override;

			inline void setEventCallback(const EventCallback& callback) override {
				data.callback = callback;
			}

			void setVSyncState(bool state) override;

			void update() override;
	};
}