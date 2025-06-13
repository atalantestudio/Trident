#pragma once

#include "Engine/base.hpp"
#include "Engine/config.hpp"

extern "C" {
	#if PREFERRED_GPU_TYPE == GPU_TYPE_DISCRETE
		// NOTE: AmdPowerXpressRequestHighPerformance seems to be recognized even on a laptop with no AMD card.
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

		__declspec(dllexport) unsigned int NvOptimusEnablement = 1;
	#endif
}

#ifdef PLATFORM_WINDOWS
	extern ProjectA::Application* createApplication();

	int main(int argc, char** argv) {
		ProjectA::Application* application;

		{
			PROFILER("Log/ProfileStart.json");

			application = ProjectA::createApplication();
			application->postConstruct();
		}

		{
			PROFILER("Log/ProfileRun.json");

			application->loop();
		}

		{
			PROFILER("Log/ProfileEnd.json");

			delete application;
		}

		return EXIT_SUCCESS;
	}
#else
	#error "Unhandled platform."
#endif