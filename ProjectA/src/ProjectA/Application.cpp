#include "pch.hpp"
#include "Application.hpp"
#include <glad/glad.h>

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application::Application() {
		CORE_ASSERT(!instance, "The Application instance already exists.")

		instance = this;

		window = std::unique_ptr<WindowInterface>(WindowInterface::create());
		window->setEventCallback(BIND_FUNCTION(Application::listen));

		imGuiLayer = new ImGuiLayer();

		pushOverlay(imGuiLayer);
	}

	Application::~Application() {}

	bool Application::windowCloseEventCallback(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	void Application::run() {
		while (running) {
			glClearColor(1, .2, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : layerStack) {
				layer->update();
			}

			imGuiLayer->begin();

			for (Layer* layer : layerStack) {
				layer->renderImGui();
			}

			imGuiLayer->end();

			window->update();
		}
	}

	void Application::listen(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<WindowCloseEvent>(BIND_FUNCTION(Application::windowCloseEventCallback));

		LOG_CORE_TRACE("{0}", event);

		for (auto i = layerStack.end(); i != layerStack.begin();) {
			(*--i)->listen(event);

			if (event.handled) {
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer) {
		layerStack.pushLayer(layer);

		layer->attach();
	}

	void Application::pushOverlay(Layer* overlay) {
		layerStack.pushOverlay(overlay);

		overlay->attach();
	}
}