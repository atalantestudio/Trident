// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ProjectType : uint8 {
		// Graphical applications (software, games, etc.).
		MAIN = 0,

		// Build tools do not use assets and don't require an Asset directory.
		BUILD = 1,
	};
}