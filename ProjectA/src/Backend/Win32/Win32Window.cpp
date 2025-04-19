#include "pch.hpp"

#include "Backend/Win32/Win32Window.hpp"

#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	/*static uint8 glfwWindowCount = 0;

	static void glfwErrorCallback(sint32 error, const char* description) {
		ENGINE_ERROR("GLFW error ([]): []", error, description);
	}

	Win32Window::Win32Window(const WindowOldDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		GLFWData& glfwData = this->data;

		glfwData.title = descriptor.name;
		glfwData.width = descriptor.width;
		glfwData.height = descriptor.height;

		if (glfwWindowCount == 0) {
			sint32 success;

			{
				PROFILE_SCOPE("Initialize GLFW");

				success = glfwInit();
			}

			ENGINE_ASSERT(success, "Failed to initialize GLFW.");

			glfwSetErrorCallback(glfwErrorCallback);
		}

		{
			PROFILE_SCOPE("Create GLFW window");

			if (Renderer::getGraphicsApi() == GraphicsAPI::OPENGL) {
				// Request OpenGL >= 4.6 (SPIR-V support).
				// Request OpenGL >= 4.3 (compute shader support).
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);				

				#ifdef BUILD_MODE_DEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
				#else
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
				#endif
			}
			else if (Renderer::getGraphicsApi() == GraphicsAPI::VULKAN) {
				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			}

			window = glfwCreateWindow(glfwData.width, glfwData.height, glfwData.title.c_str(), nullptr, nullptr);

			glfwWindowCount += 1;
		}

		glfwSetWindowUserPointer(window, &glfwData);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, sint32 width, sint32 height) {
			GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);

			data.callback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			WindowCloseEvent event;

			data.callback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, sint32 keyCode, sint32 scancode, sint32 action, sint32 mods) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					KeyPressEvent event(static_cast<KeyCode>(keyCode), 0);

					data.callback(event);

					break;
				}

				case GLFW_RELEASE: {
					KeyReleaseEvent event(static_cast<KeyCode>(keyCode));

					data.callback(event);

					break;
				}

				case GLFW_REPEAT: {
					KeyPressEvent event(static_cast<KeyCode>(keyCode), 1);

					data.callback(event);

					break;
				}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, uint32 keyCode) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			KeyTypeEvent event(static_cast<KeyCode>(keyCode));

			data.callback(event);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, sint32 button, sint32 action, sint32 mods) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressEvent event(static_cast<MouseButton>(button));

					data.callback(event);

					break;
				}

				case GLFW_RELEASE: {
					MouseButtonReleaseEvent event(static_cast<MouseButton>(button));

					data.callback(event);

					break;
				}
			}
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, float64 xpos, float64 ypos) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			MouseMoveEvent event(static_cast<sint32>(xpos), static_cast<sint32>(ypos));

			data.callback(event);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, float64 xoffset, float64 yoffset) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			MouseScrollEvent event(static_cast<float32>(xoffset), static_cast<float32>(yoffset));

			data.callback(event);
		});
	}

	Win32Window::~Win32Window() {
		PROFILE_SCOPE(FUNC_SIG);

		glfwDestroyWindow(window);

		glfwWindowCount -= 1;

		if (glfwWindowCount == 0) {
			ENGINE_TRACE("Terminating GLFW.");

			glfwTerminate();
		}
	}

	void Win32Window::update() {
		PROFILE_SCOPE(FUNC_SIG);

		glfwPollEvents();
	}*/
}