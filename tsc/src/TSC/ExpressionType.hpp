#pragma once

namespace ProjectA {
	enum class ExpressionType : uint8 {
		NUMBER,
		IDENTIFIER,
		UNARY,
		BINARY,
		GROUP,
		MEMBER_ACCESS,
		FUNCTION_CALL,
		TYPE,
	};
}