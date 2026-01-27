// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

#include "Process/DefaultProcess.hpp"

namespace Trident {
	class ProcessApplication : public Application {
		public:
			ProcessApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerProcess<DefaultProcess>();
			}
	};

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new ProcessApplication(projectDescriptor);
	}
}