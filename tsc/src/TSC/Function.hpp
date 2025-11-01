#pragma once

#include "PackedType.hpp"

namespace ProjectA {
	struct FunctionOverloadParameter {
		PackedType type;
	};

	struct FunctionOverload {
		std::vector<FunctionOverloadParameter> parameters;
	};

	struct Function {
		// TODO
		// PackedType returnType;

		uint32 identifierIndex;
		std::vector<FunctionOverload> overloads;
	};
}