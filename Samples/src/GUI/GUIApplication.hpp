#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DropDownMenuLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<DropDownMenuLayer>();
			}

			~GUIApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Lorem Ipsum";

		return new GUIApplication(applicationManifest);
	}
}