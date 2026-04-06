// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Base/VersionNumber.hpp"

namespace Trident {
	struct InstanceDescriptor {
		view<char8> projectName;
		VersionNumber projectVersion;
	};
}