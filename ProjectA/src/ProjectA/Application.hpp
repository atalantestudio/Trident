#pragma once

#include "core.hpp"

namespace ProjectA {
	class API Application {
		public:
			Application();
			virtual ~Application();

			void run();
	};

	Application* createApplication();
}