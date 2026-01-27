#pragma once

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUI/StatisticsLayer.hpp"
#include "Layer/Sample/Stacking.hpp"

namespace Trident {
	class Physics2DApplication : public Application {
		public:
			explicit Physics2DApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerLayer<SampleLayer>();
				registerLayer<StatisticsLayer>();
			}
	};

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new Physics2DApplication(projectDescriptor);
	}
}