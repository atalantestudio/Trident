#pragma once

#include "core.hpp"
#include "WindowInterface.hpp"

namespace ProjectA {
	class API Application {
		private:
			std::unique_ptr<WindowInterface> window;
			bool running = true;

		public:
			Application();

			virtual ~Application();

			void run();
	};

	Application* createApplication();
}