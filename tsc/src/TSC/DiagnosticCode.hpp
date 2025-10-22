#pragma once

namespace ProjectA {
	enum class DiagnosticCode : uint8 {
		ERROR_UNEXPECTED_CHARACTER,
		ERROR_REDEFINITION,
		WARNING_EMPTY_STRUCTURE,
	};
}