#include "pch.hpp"
#include "Application.hpp"
#include "Renderer/Renderer.hpp"
#include <GLFW/glfw3.h>

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application::Application() : lastFrameTime(0.0f) {
		CORE_ASSERT(!instance, "The Application instance already exists.")

		instance = this;

		window = std::unique_ptr<WindowInterface>(WindowInterface::create());
		window->setEventCallback(BIND(Application::listen));

		Renderer::initialize();

		imGuiLayer = new ImGuiLayer();

		pushOverlay(imGuiLayer);
	}

	void Application::run() {
		while (running) {
			float time = (float) glfwGetTime();
			float deltaTime = time - lastFrameTime;

			lastFrameTime = time;

			if (!minimized) {
				for (Layer* layer : layerStack) {
					layer->update(deltaTime);
				}
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
		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowCloseEvent>(BIND(Application::onWindowClose));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(Application::onWindowResize));

		for (auto i = layerStack.end(); i != layerStack.begin();) {
			(*--i)->listen(event);

			if (event.handled) {
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer) {
		layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* overlay) {
		layerStack.pushOverlay(overlay);
	}

	bool Application::onWindowClose(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event) {
		if (event.getWidth() == 0 || event.getHeight() == 0) {
			minimized = true;

			return false;
		}

		minimized = false;

		Renderer::onWindowResize(event.getWidth(), event.getHeight());

		return false;
	}
}