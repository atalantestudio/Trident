#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Engine/Application.hpp"
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application::Application(const std::string& name) : lastFrameTime(0.0f) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(!instance, "An application instance already exists.")

		instance = this;

		window = Window::create(WindowData(name));
		window->setEventCallback(BIND(Application::onEvent));

		Renderer::initialize();

		Renderer2D::construct();

		imGuiLayer = new ImGuiLayer();

		pushOverlay(imGuiLayer);
	}

	Application::~Application() {
		PROFILE_SCOPE(FUNC_SIG);

		Renderer::destroy();
		Renderer2D::destroy();
	}

	Window& Application::getWindow() {
		return *window;
	}

	void Application::run() {
		PROFILE_SCOPE(FUNC_SIG);

		while (running) {
			PROFILE_SCOPE("Run loop");

			const float32 time = static_cast<float32>(glfwGetTime());
			const float32 deltaTime = time - lastFrameTime;

			lastFrameTime = time;

			if (!minimized) {
				PROFILE_SCOPE("Update layers");

				for (Layer* layer : layerStack) {
					layer->update(deltaTime);
				}
			}

			imGuiLayer->begin();

			{
				PROFILE_SCOPE("Render layer ImGuis");

				for (Layer* layer : layerStack) {
					layer->renderImGui();
				}
			}

			imGuiLayer->end();
			window->update();
		}
	}

	void Application::pushLayer(Layer* layer) {
		PROFILE_SCOPE(FUNC_SIG);

		layerStack.pushLayer(layer);

		layer->bind();
	}

	void Application::pushOverlay(Layer* overlay) {
		PROFILE_SCOPE(FUNC_SIG);

		layerStack.pushOverlay(overlay);

		overlay->bind();
	}

	void Application::close() {
		running = false;
	}

	void Application::onEvent(Event& event) {
		PROFILE_SCOPE(FUNC_SIG);

		EventDispatcher eventDispatcher(event);

		eventDispatcher.dispatch<WindowCloseEvent>(BIND(Application::onWindowClose));
		eventDispatcher.dispatch<WindowResizeEvent>(BIND(Application::onWindowResize));

		for (std::vector<Layer*>::reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); ++i) {
			if (event.handled) {
				break;
			}

			(*i)->onEvent(event);
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& event) {
		running = false;

		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& event) {
		PROFILE_SCOPE(FUNC_SIG);

		if (event.getWidth() == 0 || event.getHeight() == 0) {
			minimized = true;

			return false;
		}

		minimized = false;

		Renderer::onWindowResize(event.getWidth(), event.getHeight());

		return false;
	}
}