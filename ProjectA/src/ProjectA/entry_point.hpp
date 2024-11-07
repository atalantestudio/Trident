#pragma once

#include "ProjectA/Profile/Profiler.hpp"

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main() {
		ProjectA::Logger::initialize();

		ProjectA::Application* application;

		{
			ProjectA::Profiler profiler("Log/ProfileStart.json");

			application = ProjectA::createApplication();
		}

		{
			ProjectA::Profiler profiler("Log/ProfileRun.json");

			application->run();
		}

		{
			ProjectA::Profiler profiler("Log/ProfileEnd.json");

			delete application;
		}
	}
#endif