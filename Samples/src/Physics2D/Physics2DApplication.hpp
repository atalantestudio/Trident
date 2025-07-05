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
				// TODO: On Vulkan, a framebuffer becomes invalid when registering both layers and resizing the window.
				registerLayer<RendererLayer>();
				registerLayer<StatisticsLayer>();
			}

			~Physics2DApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationManifest applicationManifest{};
		applicationManifest.projectName = "2D physics";

		return new Physics2DApplication(applicationManifest);
	}
}