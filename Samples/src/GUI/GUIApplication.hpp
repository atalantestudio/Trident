#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/SampleLayer.hpp"

namespace ProjectA {
	class GUIApplication : public Application {
		public:
			explicit GUIApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerLayer<SampleLayer>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "ProjectA Sample";
		projectDescriptor.version = {1, 0, 0};

		return new GUIApplication(projectDescriptor);
	}
}