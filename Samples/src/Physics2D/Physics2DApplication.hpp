#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUI/StatisticsLayer.hpp"
#include "Layer/Demo1Layer.hpp"

namespace ProjectA {
	class Physics2DApplication : public Application {
		public:
			Physics2DApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<Demo1Layer>();
				registerLayer<StatisticsLayer>();
			}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Physics";

		return new Physics2DApplication(applicationManifest);
	}
}