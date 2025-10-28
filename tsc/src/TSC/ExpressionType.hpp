#pragma once

namespace ProjectA {
	enum class ExpressionType : uint8 {
		LITERAL,
		IDENTIFIER,
		UNARY,
		BINARY,
		GROUP,
		MEMBER_ACCESS,
		CALL,
		TYPE,
	};
}