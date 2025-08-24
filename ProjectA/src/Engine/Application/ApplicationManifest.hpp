#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace ProjectA {
	// TODO(review): Rename to ProjectManifest?
	struct ApplicationManifest {
		ProjectType projectType;
		std::string_view projectName;
		VersionNumber projectVersion = {1, 0, 0};

		// TODO(review): Handle custom window icon.
	};
}