#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUI/StatisticsLayer.hpp"
#include "Layer/RendererLayer.hpp"

namespace ProjectA {
	class Physics2DApplication : public Application {
		public:
			Physics2DApplication(const ApplicationManifest& applicationManifest) :
				Application(applicationManifest)
			{
				registerLayer<RendererLayer>();
				registerLayer<StatisticsLayer>();
			}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "Physics";

		return new Physics2DApplication(applicationManifest);
	}
}