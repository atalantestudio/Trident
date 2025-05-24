#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUI/StatisticsLayer.hpp"
#include "Layer/RendererLayer.hpp"

namespace ProjectA {
	class Physics2DApplication : public Application {
		public:
			Physics2DApplication(const ApplicationDescriptor& applicationDescriptor) :
				Application(applicationDescriptor)
			{
				registerLayer<RendererLayer>();
				//registerLayer<StatisticsLayer>();
			}

			~Physics2DApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationDescriptor applicationDescriptor{};
		applicationDescriptor.projectName = "2D physics";

		return new Physics2DApplication(applicationDescriptor);
	}
}