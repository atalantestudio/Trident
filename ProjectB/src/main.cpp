// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/entry_point.hpp"

#include "Application.hpp"

Trident::Application* Trident::createApplication(uint32 argumentCount, char8** argumentValues) {
	const ProjectDescriptor projectDescriptor = {
		.type = ProjectType::MAIN,
		.name = "ProjectB",
		.version = {0, 0, 0},
	};

	return new ProjectB::Application(projectDescriptor, argumentCount, argumentValues);
}