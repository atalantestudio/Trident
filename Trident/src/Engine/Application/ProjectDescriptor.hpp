// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace Trident {
	struct ProjectDescriptor {
		ProjectType type;
		view<char8> name;
		VersionNumber version;

		// TODO
		// Handle custom window icon.
		// See https://learn.microsoft.com/en-us/windows/win32/menurc/using-icons and
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagea.
	};
}