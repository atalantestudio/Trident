#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/SampleLayer.hpp"

namespace ProjectA {
	class SampleApplication : public Application {
		public:
			explicit SampleApplication(const ProjectDescriptor& projectDescriptor) :
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

		return new SampleApplication(projectDescriptor);
	}
}