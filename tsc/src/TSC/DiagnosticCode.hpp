#pragma once

namespace ProjectA {
	enum class DiagnosticCode : uint8 {
		// Errors
		UNEXPECTED_CHARACTER,
		REDEFINITION,
		INVALID_TEMPLATE_ARGUMENT,
		UNDEFINED_TYPE,
		INCOMPLETE_TYPE,

		// Warnings
		EMPTY_STRUCTURE,
		INVALID_MEMBER_IDENTIFIER_CASE,
	};
}