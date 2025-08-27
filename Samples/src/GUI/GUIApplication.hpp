#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/TextLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<TextLayer>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectType = ProjectType::MAIN;
		applicationManifest.projectName = "Lorem Ipsum";

		return new GUIApplication(applicationManifest);
	}
}