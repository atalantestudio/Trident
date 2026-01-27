// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Trident.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/SampleLayer.hpp"

namespace Trident {
	class SampleApplication : public Application {
		public:
			explicit SampleApplication(const ProjectDescriptor& projectDescriptor) :
				Application(projectDescriptor)
			{
				registerLayer<SampleLayer>();
			}
	};

	Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
		ProjectDescriptor projectDescriptor{};
		projectDescriptor.type = ProjectType::MAIN;
		projectDescriptor.name = "Trident Sample";
		projectDescriptor.version = {1, 0, 0};

		return new SampleApplication(projectDescriptor);
	}
}