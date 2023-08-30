#include "pch.hpp"
#include "Window.hpp"

namespace ProjectA {
	static bool glfwInitialized = false;

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
		glfwSwapInterval(state ? 1 : 0);

		data.vSync = state;
	}

	void Window::initialize(const WindowData& data) {
		GLFWData& glfwData = this->data;

		glfwData.title = data.title;
		glfwData.width = data.width;
		glfwData.height = data.height;

		LOG_CORE_TRACE("Created {0}x{1} \"{2}\" window", glfwData.width, glfwData.height, glfwData.title);

		if (!glfwInitialized) {
			int success = glfwInit();

			CORE_ASSERT(success, "Could not initialize GLFW");

			glfwInitialized = true;
		}

		window = glfwCreateWindow((int) glfwData.width, (int) glfwData.height, glfwData.title.c_str(), nullptr, nullptr);

		glfwMakeContextCurrent(window);
		glfwSetWindowUserPointer(window, &glfwData);

		setVSyncState(true);
	}

	void Window::terminate() {
		glfwDestroyWindow(window);
	}

	void Window::update() {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}