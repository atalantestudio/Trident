#pragma once

#include <glfw/glfw3.h>

#include "Engine/API/Event/KeyEvent.hpp"
#include "Engine/API/Event/MouseEvent.hpp"
#include "Engine/API/Event/WindowEvent.hpp"
#include "Engine/WindowOld.hpp"

namespace ProjectA {
	class Win32Window : public WindowOld {
		public:
			Win32Window(const WindowOldDescriptor& descriptor);

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
			GLFWData data;
	};
}