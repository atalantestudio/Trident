#pragma once

#include "ExpressionType.hpp"

namespace ProjectA {
	struct ExpressionReference {
		ExpressionType type;
		uint32 byteOffset;
	};

	template<ExpressionType ExpressionType>
	struct Expression;

	template<>
	struct Expression<ExpressionType::LITERAL> {
		// TODO: Replace by a literal type + literal index.
		uint32 tokenIndex;
	};

	template<>
	struct Expression<ExpressionType::IDENTIFIER> {
		// TODO: Replace by an identifier index.
		uint32 tokenIndex;
	};

	template<>
	struct Expression<ExpressionType::UNARY> {
		// TODO: Replace by an unary operator index.
		uint32 operatorTokenIndex;

		// The right expression directly follows.
	};

	template<>
	struct Expression<ExpressionType::BINARY> {
		// TODO: Replace by a binary operator index.
		uint32 operatorTokenIndex;

		// The left and right expressions directly follow.
	};

	template<>
	struct Expression<ExpressionType::GROUP> {
		// The expression directly follows.
	};

	template<>
	struct Expression<ExpressionType::MEMBER_ACCESS> {
		uint32 identifierTokenIndex;

		// TODO: The right expression directly follows.
	};

	template<>
	struct Expression<ExpressionType::CALL> {
		// If true, the following expression is the callee.
		// It can either be a type expression (constructor call) or an identifier expression (function call).
		bool explicitCallee;

		// The argument expressions follow this expression and the optional callee expression.
		uint32 argumentCount;
	};

	template<>
	struct Expression<ExpressionType::TYPE> {
		uint32 typeTokenIndex;
		uint32 templateArgumentTokenIndex = static_cast<uint32>(~0);
	};
}