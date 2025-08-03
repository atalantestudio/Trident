#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/SampleLayer.hpp"

namespace ProjectA {
	class SampleApplication : public Application {
		public:
			SampleApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<SampleLayer>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Sample";

		return new SampleApplication(applicationManifest);
	}
}