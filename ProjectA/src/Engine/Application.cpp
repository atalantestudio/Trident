#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Engine/Application.hpp"
#include "Engine/Renderer/2D/Renderer2D.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	Application* Application::instance = nullptr;

	Application& Application::get() {
		return *instance;
	}

	Application::Application(const std::string& name) {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(!instance, "An application instance already exists.");

		instance = this;

		Renderer::setGraphicsApi(GraphicsAPI::VULKAN);

		constexpr uint32 extentWidth = 1280;
		constexpr uint32 extentHeight = 720;

		// Initialize GLFW and create window.
		window = Window::create(WindowData(name, extentWidth, extentHeight));
		window->setEventCallback(BIND(Application::onEvent));

		// Initialize graphics API.
		Renderer::registerWindow(static_cast<GLFWwindow*>(window->getNativeWindow()));
		Renderer::recreate();

		const RenderPassDescriptor renderPassDescriptor;

		renderPass = Renderer::getBackend()->createRenderPass(renderPassDescriptor);

		SwapchainDescriptor swapchainDescriptor;
		swapchainDescriptor.renderPass = renderPass;
		swapchainDescriptor.extentWidth = extentWidth;
		swapchainDescriptor.extentHeight = extentHeight;

		swapchain = Renderer::getBackend()->createSwapchain(swapchainDescriptor);

		imGuiLayer = new ImGuiLayer();

		pushOverlay(imGuiLayer);
	}

	Application::~Application() {
		PROFILE_SCOPE(FUNC_SIG);
	}

	Window& Application::getWindow() {
		return *window;
	}

	const ref<RenderPass>& Application::getRenderPass() const {
		return renderPass;
	}

	const ref<Swapchain>& Application::getSwapchain() const {
		return swapchain;
	}

	void Application::run() {
		PROFILE_SCOPE(FUNC_SIG);

		// TODO: Use finite loop?
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
				PROFILE_SCOPE("Render layer ImGui");

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

		for (chain<Layer*>::reverse_iterator i = layerStack.rbegin(); i != layerStack.rend(); i += 1) {
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

		//Renderer::onWindowResize(event.getWidth(), event.getHeight());

		return false;
	}
}