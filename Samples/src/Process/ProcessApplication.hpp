#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/TTFToTFOProcess.hpp"

namespace ProjectA {
	class ProcessApplication : public Application {
		public:
			ProcessApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerProcess<TTFToTFOProcess>();
			}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Process";

		return new ProcessApplication(applicationManifest);
	}
}