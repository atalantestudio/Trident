#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "LayerStack.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class API Application {
		private:
			std::unique_ptr<WindowInterface> window;
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
	};

	Application* createApplication();
}