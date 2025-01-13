#pragma once

#include "ProjectA/core.hpp"

#ifdef WINDOWS
	extern ProjectA::Application* ProjectA::createApplication();

	int main(int argc, char** argv) {
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

		return 0;
	}
#endif