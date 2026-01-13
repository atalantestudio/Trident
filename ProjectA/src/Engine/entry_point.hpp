// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Exports.hpp"

extern ProjectA::Application* createApplication(ProjectA::uint32 argumentCount, ProjectA::char8** argumentValues);

int main(int argc, char** argv) {
	ProjectA::Application* application;

	{
		PROFILER("Trace/ApplicationStart.json");

		application = ProjectA::createApplication(static_cast<ProjectA::uint32>(argc), argv);
		application->postConstruct();
	}

	{
		PROFILER("Trace/ProcessRun.json");

		const int code = application->runProcesses();

		if (code != 0) {
			ENGINE_DEBUG("Application exited with code [].", code);

			return code;
		}
	}

	if (application->hasLayers()) {
		PROFILER("Trace/ApplicationRun.json");

		application->loop();
	}

	{
		PROFILER("Trace/ApplicationEnd.json");

		delete application;
	}

	return EXIT_SUCCESS;
}