#include "pch.hpp"

#include "Platform/Windows/Window.hpp"

namespace ProjectA {
	static std::uint8_t glfwWindowCount = 0;

	static void glfwErrorCallback(int error, const char* description) {
		ENGINE_ERROR("GLFW error ([]): []", error, description);
	}

	Scope<WindowInterface> WindowInterface::create(const WindowData& data) {
		return CreateScope<Window>(data);
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

		ENGINE_TRACE("Created []x[] \"[]\" window", glfwData.width, glfwData.height, glfwData.title);

		if (glfwWindowCount == 0) {
			int success;

			{
				PROFILE_SCOPE("glfwInit");

				success = glfwInit();
			}

			ENGINE_ASSERT(success, "Could not initialize GLFW");

			glfwSetErrorCallback(glfwErrorCallback);
		}

		{
			PROFILE_SCOPE("glfwCreateWindow");

			window = glfwCreateWindow((int) glfwData.width, (int) glfwData.height, glfwData.title.c_str(), nullptr, nullptr);

			glfwWindowCount++;
		}

		context = Context::create(window);
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

		glfwWindowCount--;

		if (glfwWindowCount == 0) {
			ENGINE_INFO("Terminating GLFW.");

			glfwTerminate();
		}
	}

	void Window::update() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwPollEvents();

		context->swapBuffers();
	}
}