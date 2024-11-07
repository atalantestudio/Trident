#pragma once

#include "ProjectA/Profile/Profiler.hpp"

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main() {
		ProjectA::Logger::initialize();

		ProjectA::Application* application;

		{
			PROFILER("Log/ProfileStart.json");

			application = ProjectA::createApplication();
		}

		{
			PROFILER("Log/ProfileRun.json");

			application->run();
		}

		{
			PROFILER("Log/ProfileEnd.json");

			delete application;
		}
	}
#endif