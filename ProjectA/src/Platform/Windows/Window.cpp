#include "pch.hpp"
#include "Platform/OpenGL/OpenGLContext.hpp"
#include "ProjectA/Event/KeyEvent.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "Window.hpp"
#include <glad/glad.h>

namespace ProjectA {
	static bool glfwInitialized = false;

	static void glfwErrorCallback(int error, const char* description) {
		LOG_CORE_ERROR("GLFW error ({0}): {1}", error, description);
	}

	WindowInterface* WindowInterface::create(const WindowData& data) {
		return new Window(data);
	}

	Window::Window(const WindowData& data) {
		initialize(data);
	}

	Window::~Window() {
		terminate();
	}

	bool Window::isVSyncEnabled() const {
		return data.vSync;
	}

	void Window::setVSyncState(bool state) {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwSwapInterval(state ? 1 : 0);

		data.vSync = state;
	}

	void Window::initialize(const WindowData& data) {
		PROFILE_SCOPE(__FUNCSIG__);

		GLFWData& glfwData = this->data;

		glfwData.title = data.title;
		glfwData.width = data.width;
		glfwData.height = data.height;

		LOG_CORE_TRACE("Created {0}x{1} \"{2}\" window", glfwData.width, glfwData.height, glfwData.title);

		if (!glfwInitialized) {
			int success;

			{
				PROFILE_SCOPE("glfwInit");

				success = glfwInit();
			}

			CORE_ASSERT(success, "Could not initialize GLFW");

			glfwSetErrorCallback(glfwErrorCallback);

			glfwInitialized = true;
		}

		{
			PROFILE_SCOPE("glfwCreateWindow");

			window = glfwCreateWindow((int) glfwData.width, (int) glfwData.height, glfwData.title.c_str(), nullptr, nullptr);
		}

		context = new OpenGLContext(window);
		context->initialize();

		glfwSetWindowUserPointer(window, &glfwData);

		setVSyncState(true);

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);

			data.callback(event);
		});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			WindowCloseEvent event;

			data.callback(event);
		});

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					KeyPressEvent event(key, 0);

					data.callback(event);

					break;
				}

				case GLFW_RELEASE: {
					KeyReleaseEvent event(key);

					data.callback(event);

					break;
				}

				case GLFW_REPEAT: {
					KeyPressEvent event(key, 1);

					data.callback(event);

					break;
				}
			}
		});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keyCode) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			KeyTypeEvent event(keyCode);

			data.callback(event);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS: {
					MouseButtonPressEvent event(button);

					data.callback(event);

					break;
				}

				case GLFW_RELEASE: {
					MouseButtonReleaseEvent event(button);

					data.callback(event);

					break;
				}
			}
		});

		glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			MouseMoveEvent event((float) xpos, (float) ypos);

			data.callback(event);
		});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
			GLFWData& data = *(GLFWData*) glfwGetWindowUserPointer(window);

			MouseScrollEvent event((float) xoffset, (float) yoffset);

			data.callback(event);
		});
	}

	void Window::terminate() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwDestroyWindow(window);
	}

	void Window::update() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwPollEvents();

		context->swapBuffers();
	}
}