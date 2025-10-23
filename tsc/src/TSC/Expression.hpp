#pragma once

#include "ExpressionType.hpp"

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
	struct Expression<ExpressionType::IDENTIFIER> {
		uint32 identifierTokenIndex;
	};

	template<>
	struct Expression<ExpressionType::NUMBER> {
		uint32 numberTokenIndex;
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
	struct Expression<ExpressionType::FUNCTION_CALL> {
		uint32 calleeExpressionReferenceIndex;
		std::vector<uint32> argumentExpressionReferenceIndices;
	};

	template<>
	struct Expression<ExpressionType::TYPE> {
		uint32 typeTokenIndex;
		uint32 templateArgumentTokenIndex = UNDEFINED_TOKEN;
	};
}