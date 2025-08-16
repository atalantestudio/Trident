#pragma once

namespace ProjectA {
	enum class ProjectType : uint8 {
		// Standard type, used for software, games, etc.
		MAIN = 0,

		// Build tools do not use assets and don't require an Asset directory.
		BUILD = 1,
	};
}