#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/MyLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			GUIApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerLayer<MyLayer>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Lorem Ipsum";
		projectDescriptor.version = {1, 0, 0};

		return new GUIApplication(projectDescriptor);
	}
}