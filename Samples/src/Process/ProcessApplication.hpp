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

	Application* ProjectA::createApplication(uint32 argumentCount, char** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Process";

		return new ProcessApplication(applicationManifest);
	}
}