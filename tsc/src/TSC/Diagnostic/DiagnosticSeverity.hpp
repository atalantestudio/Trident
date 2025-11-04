#pragma once

namespace ProjectA {
	enum class DiagnosticSeverity : uint8 {
		ERROR,
		WARNING,
		INFO,

		// TODO: Remove suffix.
		HINT_OR_WIP,
	};
}