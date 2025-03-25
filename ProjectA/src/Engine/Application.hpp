#pragma once

#include "Engine/base.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/ImGui/ImGuiLayer.hpp"
#include "Engine/Layer/LayerStack.hpp"
#include "Engine/Renderer/Swapchain.hpp"
#include "Engine/Window.hpp"

int main(int argc, char** argv);

namespace ProjectA {
	class Application {
		public:
			static Application& get();

		public:
			Application(const std::string& name = "Application");

			virtual ~Application();

			Window& getWindow();

			const ref<RenderPass>& getRenderPass() const;

			const ref<Swapchain>& getSwapchain() const;

			void onEvent(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			void close();

		private:
			void run();

			bool onWindowClose(WindowCloseEvent& event);
			bool onWindowResize(WindowResizeEvent& event);

		private:
			friend int ::main(int argc, char** argv);

		public:
			ImGuiLayer* imGuiLayer;

		private:
			static Application* instance;

		private:
			scope<Window> window;
			LayerStack layerStack;

			// TODO: Use scope here?
			ref<RenderPass> renderPass;
			ref<Swapchain> swapchain;

			bool running = true;
			bool minimized = false;
			float32 lastFrameTime = 0.0f;
	};

	Application* createApplication();
}