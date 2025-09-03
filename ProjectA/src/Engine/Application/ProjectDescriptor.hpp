#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace ProjectA {
	struct ProjectDescriptor {
		ProjectType type;
		std::string_view name;
		VersionNumber version;

		// TODO(review): Handle custom window icon.
	};
}