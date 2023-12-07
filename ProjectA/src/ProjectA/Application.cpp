#include "pch.hpp"
#include "Application.hpp"
#include <GLFW/glfw3.h>

namespace ProjectA {
	Application::Application() {
		window = std::unique_ptr<WindowInterface>(WindowInterface::create());
		window->setEventCallback(BIND_FUNCTION(Application::eventCallback));
	}

	Application::~Application() {}

	void Application::run() {
		while (running) {
			glClearColor(1, .2, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			window->update();
		}
	}

	void Application::eventCallback(Event& event) {
		LOG_CORE_TRACE("{0}", event);

		EventDispatcher dispatcher(event);
		dispatcher.dispatch<WindowCloseEvent>(BIND_FUNCTION(Application::windowCloseEventCallback));
	}

	bool Application::windowCloseEventCallback(WindowCloseEvent& event) {
		running = false;

		return true;
	}
}