#pragma once

extern "C" {
	#ifdef GL_PREFER_INTEGRATED_GPU
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

	{
		PROFILER("Trace/processes.json");

		const int code = application->runProcesses();

		if (code != 0) {
			ENGINE_DEBUG("Application exited with code [].", code);

			return code;
		}
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