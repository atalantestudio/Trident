#pragma once

#include "PackedType.hpp"

namespace ProjectA {
	struct ExpressionEvaluation {
		// The type the expression must evaluate to in the end.
		PackedType type;

		// An intermediate type the expression is explicitly allowed to evaluate to while being analyzed.
		PackedType intermediateType;

		std::vector<sint64> components = {0};

		bool casting;
		bool constant;
	};
}