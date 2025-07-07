#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/MyLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<MyLayer>();
			}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Lorem Ipsum";

		return new GUIApplication(applicationManifest);
	}
}