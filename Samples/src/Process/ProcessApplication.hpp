#pragma once

#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Process/DefaultProcess.hpp"

namespace ProjectA {
	class ProcessApplication : public Application {
		public:
			ProcessApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerProcess<DefaultProcess>();
			}
	};

	Application* ProjectA::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "ProjectA Sample";
		projectDescriptor.version = {1, 0, 0};

		return new ProcessApplication(projectDescriptor);
	}
}