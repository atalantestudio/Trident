#pragma once

namespace ProjectA {
	enum class StatementType : uint8 {
		STRUCTURE,
		STRUCTURE_MEMBER,
		GLOBAL_SCOPE_CONSTANT,

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