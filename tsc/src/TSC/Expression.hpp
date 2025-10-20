#pragma once

#include "ExpressionType.hpp"

namespace ProjectA {
	struct Expression {
		explicit Expression(ExpressionType type) :
			type(type)
		{}

		ExpressionType type;
	};

	template<ExpressionType ExpressionType>
	struct BaseExpression : public Expression {
		explicit BaseExpression() :
			Expression(ExpressionType)
		{}
	};

	struct IdentifierExpression : public BaseExpression<ExpressionType::IDENTIFIER> {
		uint32 identifierTokenIndex;
	};

	struct NumberExpression : public BaseExpression<ExpressionType::NUMBER> {
		uint32 numberTokenIndex;
	};

	struct UnaryExpression : public BaseExpression<ExpressionType::UNARY> {
		uint32 operatorTokenIndex;
		uint32 rightExpressionIndex;
	};

	struct BinaryExpression : public BaseExpression<ExpressionType::BINARY> {
		uint32 leftExpressionIndex;
		uint32 operatorTokenIndex;
		uint32 rightExpressionIndex;
	};

	struct GroupExpression : public BaseExpression<ExpressionType::GROUP> {
		uint32 expressionIndex;
	};

	struct MemberAccessExpression : public BaseExpression<ExpressionType::MEMBER_ACCESS> {
		uint32 leftExpressionIndex;
		std::string memberIdentifier;
	};

	struct FunctionCallExpression : public BaseExpression<ExpressionType::FUNCTION_CALL> {
		uint32 calleeExpressionIndex;
		std::vector<uint32> argumentExpressionIndices;
	};

	struct TypeExpression : public BaseExpression<ExpressionType::TYPE> {
		uint32 typeTokenIndex;
		uint32 templateArgumentTokenIndex = ~0;
	};
}