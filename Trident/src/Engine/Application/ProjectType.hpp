// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class ProjectType : uint8 {
		// Graphical applications (software, games, etc.).
		MAIN = 0,

		// Build tools are executed on the command-line.
		BUILD = 1,
	};
}