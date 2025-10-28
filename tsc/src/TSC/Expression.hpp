#pragma once

#include "ExpressionType.hpp"
#include "PackedType.hpp"

namespace ProjectA {
	static constexpr uint32 UNDEFINED_EXPRESSION = ~0;
	static constexpr uint32 UNDEFINED_TOKEN = ~0;

	struct ExpressionReference {
		ExpressionType type;
		uint32 byteOffset;
	};

	template<ExpressionType ExpressionType>
	struct Expression;

	template<>
	struct Expression<ExpressionType::LITERAL> {
		uint32 tokenIndex;
	};

	template<>
	struct Expression<ExpressionType::IDENTIFIER> {
		uint32 tokenIndex;
	};

	template<>
	struct Expression<ExpressionType::UNARY> {
		uint32 operatorTokenIndex;
		uint32 rightExpressionReferenceIndex;
	};

	template<>
	struct Expression<ExpressionType::BINARY> {
		uint32 leftExpressionReferenceIndex;
		uint32 operatorTokenIndex;
		uint32 rightExpressionReferenceIndex;
	};

	template<>
	struct Expression<ExpressionType::GROUP> {
		uint32 expressionReferenceIndex;
	};

	template<>
	struct Expression<ExpressionType::MEMBER_ACCESS> {
		uint32 leftExpressionReferenceIndex;
		uint32 identifierTokenIndex;
	};

	template<>
	struct Expression<ExpressionType::CALL> {
		// NOTE: This can either reference a type expression (constructor call) or an identifier expression (function call).
		uint32 calleeExpressionReferenceIndex;

		// The argument expressions are located between the callee expression and this expression.
		uint32 argumentCount;
	};

	template<>
	struct Expression<ExpressionType::TYPE> {
		PackedType type;
		uint32 typeTokenIndex;
		uint32 templateArgumentTokenIndex = UNDEFINED_TOKEN;
	};
}