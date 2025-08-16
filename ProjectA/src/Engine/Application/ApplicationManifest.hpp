#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace ProjectA {
	// TODO: Rename to ProjectManifest?
	struct ApplicationManifest {
		ProjectType projectType;
		std::string_view projectName;

		// TODO: Handle custom window icon.
	};
}