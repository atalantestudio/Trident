#pragma once

#include "PackedType.hpp"

namespace ProjectA {
	struct ExpressionEvaluation {
		PackedType intermediateType;
		sint64 value;

		bool casting;
		bool constant;
	};
}