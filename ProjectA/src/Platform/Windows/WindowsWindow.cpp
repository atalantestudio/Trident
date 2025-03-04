#include "pch.hpp"

#include "Platform/Windows/WindowsWindow.hpp"

#include "Engine/Renderer/Renderer.hpp"

#include <vulkan/vulkan.h>

namespace ProjectA {
	static uint8 glfwWindowCount = 0;

	static void glfwErrorCallback(int error, const char* description) {
		ENGINE_ERROR("GLFW error ([]): []", error, description);
	}

	WindowsWindow::WindowsWindow(const WindowData& data) {
		initialize(data);
	}

	WindowsWindow::~WindowsWindow() {
		terminate();
	}

	bool WindowsWindow::isVSyncEnabled() const {
		return data.vSync;
	}

	void WindowsWindow::setVSyncState(bool state) {
		PROFILE_SCOPE(FUNC_SIG);

		if (Renderer::getGraphicsApi() == GraphicsAPI::OPENGL) {
			glfwSwapInterval(static_cast<int32>(state));
		}
		else if (Renderer::getGraphicsApi() == GraphicsAPI::VULKAN) {
			ENGINE_WARN("No support for swap interval with a Vulkan context.");
		}

		data.vSync = state;
	}

	void WindowsWindow::initialize(const WindowData& data) {
		PROFILE_SCOPE(FUNC_SIG);

		GLFWData& glfwData = this->data;

		glfwData.title = data.title;
		glfwData.width = data.width;
		glfwData.height = data.height;

		if (glfwWindowCount == 0) {
			int32 success;

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
				// Request OpenGL >= 3.3.
				glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
				glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);				

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

			glfwWindowCount++;
		}

		context = Context::create(window);

		glfwSetWindowUserPointer(window, &glfwData);

		setVSyncState(true);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int32 width, int32 height) {
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

		glfwSetKeyCallback(window, [](GLFWwindow* window, int32 keyCode, int32 scancode, int32 action, int32 mods) {
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

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int32 button, int32 action, int32 mods) {
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

			MouseMoveEvent event(static_cast<float32>(xpos), static_cast<float32>(ypos));

			data.callback(event);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, float64 xoffset, float64 yoffset) {
			const GLFWData& data = *static_cast<GLFWData*>(glfwGetWindowUserPointer(window));

			MouseScrollEvent event(static_cast<float32>(xoffset), static_cast<float32>(yoffset));

			data.callback(event);
		});
	}

	void WindowsWindow::terminate() {
		PROFILE_SCOPE(FUNC_SIG);

		glfwDestroyWindow(window);

		glfwWindowCount--;

		if (glfwWindowCount == 0) {
			ENGINE_TRACE("Terminating GLFW...");

			glfwTerminate();
		}
	}

	void WindowsWindow::update() {
		PROFILE_SCOPE(FUNC_SIG);

		glfwPollEvents();

		context->swapBuffers();
	}
}