#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "LayerStack.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class API Application {
		private:
			static Application* instance;

			std::unique_ptr<WindowInterface> window;
			ImGuiLayer* imGuiLayer;
			bool running = true;
			LayerStack layerStack;

		private:
			bool windowCloseEventCallback(WindowCloseEvent& event);

		public:
			Application();
			virtual ~Application();

			void run();
			void listen(Event& event);
			void pushLayer(Layer* layer);
			void pushOverlay(Layer* overlay);

			inline WindowInterface& getWindow() {
				return *window;
			}

			static inline Application& get() {
				return *instance;
			}
	};

	Application* createApplication();
}