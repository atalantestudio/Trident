#pragma once

namespace ProjectA {
	enum class StatementType : uint8 {
		STRUCTURE,

		// NOTE: Used by constants, function parameters and local scope variables.
		VARIABLE,

		PARAMETER,
		FUNCTION,
		EXPRESSION,
		SCOPE,
		IF,
		WHILE,
		RETURN,
	};
}