#pragma once

#include "core.hpp"
#include "Event/WindowEvent.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class API Application {
		private:
			std::unique_ptr<WindowInterface> window;
			bool running = true;

		private:
			bool windowCloseEventCallback(WindowCloseEvent& event);

		public:
			Application();

			virtual ~Application();

			void run();
			void eventCallback(Event& event);
	};

	Application* createApplication();
}