// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Application/Application.hpp"
#include "Engine/Exports.hpp"

extern Trident::Application* createApplication(Trident::uint32 argumentCount, Trident::char8** argumentValues);

int main(int argc, char** argv) {
	Trident::Application* application;

	{
		PROFILER("Trace/ApplicationStart.json");

		application = Trident::createApplication(static_cast<Trident::uint32>(argc), argv);
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