#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace Trident {
	struct ProjectDescriptor {
		ProjectType type;
		sequence<char8> name;
		VersionNumber version;

		// TODO
		// Handle custom window icon.
		// See https://learn.microsoft.com/en-us/windows/win32/menurc/using-icons and
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagea.
	};
}