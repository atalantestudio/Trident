#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/TestLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<TestLayer>();
			}

			~GUIApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "GUI";

		return new GUIApplication(applicationManifest);
	}
}