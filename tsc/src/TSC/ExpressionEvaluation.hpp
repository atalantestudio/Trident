#pragma once

namespace ProjectA {
	struct ExpressionEvaluation {
		uint32 intermediateType = 0;
		bool constant = true;
		sint64 value = 0;

		// TODO
		// TokenType literalType;
	};
}