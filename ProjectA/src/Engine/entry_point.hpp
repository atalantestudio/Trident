#pragma once

extern "C" {
	#if PREFERRED_GPU_TYPE == GPU_TYPE_DISCRETE
		// NOTE: AmdPowerXpressRequestHighPerformance seems to be recognized even if no AMD card is available.
		__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

		__declspec(dllexport) unsigned int NvOptimusEnablement = 1;
	#endif
}

extern ProjectA::Application* createApplication(ProjectA::uint32 argumentCount, ProjectA::char8** argumentValues);

int main(int argc, char** argv) {
	ProjectA::Application* application;

	{
		PROFILER("Trace/ProfileStart.json");

		application = ProjectA::createApplication(static_cast<ProjectA::uint32>(argc), argv);
		application->postConstruct();
	}

	if (application->hasLayers()) {
		PROFILER("Trace/ProfileRun.json");

		application->loop();
	}

	{
		PROFILER("Trace/ProfileEnd.json");

		delete application;
	}

	return EXIT_SUCCESS;
}