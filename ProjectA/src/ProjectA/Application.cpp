#include "pch.hpp"
#include "Application.hpp"
#include <GLFW/glfw3.h>

namespace ProjectA {
	Application::Application() {
		window = std::unique_ptr<WindowInterface>(WindowInterface::create());
	}

	Application::~Application() {}

	void Application::run() {
		while (running) {
			glClearColor(1, .2, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			window->update();
		}
	}
}