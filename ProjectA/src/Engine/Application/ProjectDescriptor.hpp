#pragma once

#include "Engine/Application/ProjectType.hpp"

namespace ProjectA {
	struct ProjectDescriptor {
		ProjectType type;
		str8 name;
		VersionNumber version;

		// TODO
		// Handle custom window icon.
		// See https://learn.microsoft.com/en-us/windows/win32/menurc/using-icons and
		// https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-loadimagea.
	};
}