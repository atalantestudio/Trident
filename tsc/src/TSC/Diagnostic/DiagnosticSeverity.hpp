#pragma once

namespace ProjectA {
	// NOTE
	// Avoid confict with ERROR defined in Windows.h.
	#undef ERROR

	enum class DiagnosticSeverity : uint8 {
		ERROR,
		WARNING,
		INFO,
		HINT,
	};
}