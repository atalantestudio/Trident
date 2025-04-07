#pragma once

#include "Engine/base.hpp"

#ifdef PLATFORM_WINDOWS
	extern ProjectA::Application* createApplication();

	int main(int argc, char** argv) {
		ProjectA::Filesystem::initialize();

		#ifdef BUILD_MODE_DEBUG
			ProjectA::Logger::initialize();
		#endif

		ProjectA::Application* application;

		{
			PROFILER("Log/ProfileStart.json");

			application = ProjectA::createApplication();
		}

		{
			PROFILER("Log/ProfileRun.json");

			application->loop();
		}

		{
			PROFILER("Log/ProfileEnd.json");

			delete application;
		}

		return 0;
	}
#else
	#error "Unhandled platform."
#endif