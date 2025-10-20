#pragma once

namespace ProjectA {
	enum class DiagnosticSeverity : uint8 {
		ERROR,
		WARNING,
		INFO,

		// TODO: Remove _OR_WIP suffix.
		HINT_OR_WIP,
	};
}