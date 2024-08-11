#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "LayerStack.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class Application {
		private:
			static Application* instance;

			std::unique_ptr<WindowInterface> window;
			ImGuiLayer* imGuiLayer;
			bool running = true;
			LayerStack layerStack;

		private:
			bool windowCloseEventCallback(WindowCloseEvent& event);

		public:
			static inline Application& get() {
				return *instance;
			}

			Application();

			virtual ~Application() = default;

			void run();

			void listen(Event& event);

			void pushLayer(Layer* layer);

			void pushOverlay(Layer* overlay);

			inline WindowInterface& getWindow() {
				return *window;
			}
	};

	Application* createApplication();
}