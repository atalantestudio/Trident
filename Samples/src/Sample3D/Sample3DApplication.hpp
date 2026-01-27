#pragma once

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/Sample3DLayer.hpp"

namespace Trident {
	class Sample3DApplication : public Application {
		public:
			explicit Sample3DApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerLayer<Sample3DLayer>();
			}
	};

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new Sample3DApplication(projectDescriptor);
	}
}