#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/UITestLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<UITestLayer>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectType = ProjectType::MAIN;
		applicationManifest.projectName = "Lorem Ipsum";

		return new GUIApplication(applicationManifest);
	}
}