#pragma once

#include "core.hpp"
#include "Event/Event.hpp"

namespace ProjectA {
	class API Application {
		public:
			Application();
			virtual ~Application();

			void run();
	};

	Application* createApplication();
}