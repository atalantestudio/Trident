#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/DefaultProcess.hpp"

namespace ProjectA {
	class ProcessApplication : public Application {
		public:
			ProcessApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerProcess<DefaultProcess>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectType = ProjectType::MAIN;
		applicationManifest.projectName = "Process";

		return new ProcessApplication(applicationManifest);
	}
}