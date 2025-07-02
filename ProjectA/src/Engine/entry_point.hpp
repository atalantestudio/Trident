#pragma once

extern "C" {
	#if PREFERRED_GPU_TYPE == GPU_TYPE_DISCRETE
		// NOTE: AmdPowerXpressRequestHighPerformance seems to be recognized even if no AMD card is available.
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

		__declspec(dllexport) unsigned int NvOptimusEnablement = 1;
	#endif
}

extern ProjectA::Application* createApplication();

int main(int argc, char** argv) {
	ProjectA::Application* application;

	{
		PROFILER("Log/ProfileStart.json");

		application = ProjectA::createApplication();
		application->postConstruct();
	}

	if (application->hasGraphicalProcesses()) {
		PROFILER("Log/ProfileRun.json");

		application->loop();
	}

	{
		PROFILER("Log/ProfileEnd.json");

		delete application;
	}

	return EXIT_SUCCESS;
}