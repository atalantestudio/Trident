#pragma once

#include "DocumentRange.hpp"
#include "StatementType.hpp"

namespace ProjectA {
	template<StatementType StatementType>
	struct Statement;

	struct IncludeStatement {
		DocumentRange range;
		std::string path;
	};

	template<>
	struct Statement<StatementType::STRUCTURE> {
		StatementType type = StatementType::STRUCTURE;

		uint32 identifierTokenIndex;
		uint32 memberCount;
		DocumentPosition end;
	};

	template<>
	struct Statement<StatementType::MEMBER> {
		StatementType type = StatementType::MEMBER;

		uint32 annotationBlockIndexOffset;
		uint32 annotationBlockCount;
		uint32 annotationFlag;

		// The type expression is the next expression.

		uint32 identifierTokenIndex;
	};

	template<>
	struct Statement<StatementType::CONSTANT> {
		StatementType type = StatementType::CONSTANT;

		uint32 annotationBlockIndexOffset;
		uint32 annotationBlockCount;
		uint32 annotationFlag;

		// The type expression is the next expression.

		uint32 identifierTokenIndex;

		// The initializer expression is the next expression.
	};

	template<>
	struct Statement<StatementType::VARIABLE> {
		StatementType type = StatementType::VARIABLE;

		// The type expression is the next expression.

		uint32 identifierTokenIndex;
		bool constant;

		// If initializer is true, the initializer expression is the next expression.
		bool initializer;
	};

	template<>
	struct Statement<StatementType::PARAMETER> {
		StatementType type = StatementType::PARAMETER;

		// The type expression is the next expression.

		uint32 identifierTokenIndex;
	};

	template<>
	struct Statement<StatementType::FUNCTION> {
		StatementType type = StatementType::FUNCTION;

		// If returnType is true, the return type expression is the next expression.
		bool returnType;

		uint32 identifierTokenIndex;
		uint32 parameterCount;

		// The scope statement is the next statement.

		DocumentPosition end;
	};

	template<>
	struct Statement<StatementType::EXPRESSION> {
		StatementType type = StatementType::EXPRESSION;

		// The expression is the next expression.
	};

	template<>
	struct Statement<StatementType::SCOPE> {
		StatementType type = StatementType::SCOPE;

		uint32 statementCount;
		DocumentRange range;
	};

	template<>
	struct Statement<StatementType::IF> {
		StatementType type = StatementType::IF;

		// The condition expression is the next expression.
		// The scope statement is the next statement.
	};

	template<>
	struct Statement<StatementType::WHILE> {
		StatementType type = StatementType::WHILE;

		// The condition expression is the next expression.
		// The scope statement is the next statement.
	};

	template<>
	struct Statement<StatementType::BREAK> {
		StatementType type = StatementType::BREAK;
	};

	template<>
	struct Statement<StatementType::NEXT> {
		StatementType type = StatementType::NEXT;
	};

	template<>
	struct Statement<StatementType::RETURN> {
		StatementType type = StatementType::RETURN;

		// The expression is the next expression.
	};
}