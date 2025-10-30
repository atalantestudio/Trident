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
	};

	template<>
	struct Expression<ExpressionType::BINARY> {
		// TODO: Replace by an binary operator index.
		uint32 operatorTokenIndex;
	};

	template<>
	struct Expression<ExpressionType::GROUP> {};

	template<>
	struct Expression<ExpressionType::MEMBER_ACCESS> {
		// TODO: Should preceed.
		uint32 leftExpressionReferenceIndex;

		uint32 identifierTokenIndex;
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
		PackedType type;
		uint32 typeTokenIndex;
		uint32 templateArgumentTokenIndex = UNDEFINED_TOKEN;
	};
}