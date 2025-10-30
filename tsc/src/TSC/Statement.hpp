#pragma once

#include "DocumentPosition.hpp"
#include "StatementType.hpp"

namespace ProjectA {
	struct Statement {
		explicit Statement(StatementType type) :
			statementType(type)
		{}

		StatementType statementType;
	};

	template<StatementType StatementType>
	struct BaseStatement : public Statement {
		explicit BaseStatement() :
			Statement(StatementType)
		{}
	};

	struct IncludeStatement {
		DocumentPosition start;
		DocumentPosition end;
		std::string path;
	};

	struct StructureStatement : public BaseStatement<StatementType::STRUCTURE> {
		uint32 identifierTokenIndex;
		uint32 memberCount;
	};

	struct MemberStatement : public BaseStatement<StatementType::MEMBER> {
		uint32 annotationFlag;
		uint32 annotationBlockIndexOffset;
		uint32 annotationBlockCount;
		uint32 identifierTokenIndex;

		// The type expression is the next expression in the AST buffer.
	};

	struct ConstantStatement : public BaseStatement<StatementType::CONSTANT> {
		uint32 annotationFlag;
		uint32 annotationBlockIndexOffset;
		uint32 annotationBlockCount;
		uint32 identifierTokenIndex;

		// The type expression is the next expression in the AST buffer.
		// The initializer expression is the next expression in the AST buffer.
	};

	struct VariableStatement : public BaseStatement<StatementType::VARIABLE> {
		uint32 typeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
		bool constant;
		bool initializer;

		// The type expression is the next expression in the AST buffer.
		// If initializer is true, the initializer expression is the next expression in the AST buffer.
	};

	struct ParameterStatement : public BaseStatement<StatementType::PARAMETER> {
		uint32 identifierTokenIndex;

		// The type expression is the next expression in the AST buffer.
	};

	struct FunctionStatement : public BaseStatement<StatementType::FUNCTION> {
		bool returnType;
		uint32 identifierTokenIndex;
		std::vector<uint32> parameterStatementIndices;
		uint32 scopeStatementIndex;

		// If returnType is true, the return type expression is the next expression in the AST buffer.
	};

	struct ExpressionStatement : public BaseStatement<StatementType::EXPRESSION> {
		// The expression is the next expression in the AST buffer.
	};

	struct ScopeStatement : public BaseStatement<StatementType::SCOPE> {
		std::vector<uint32> statementIndices;
	};

	struct IfStatement : public BaseStatement<StatementType::IF> {
		uint32 scopeStatementIndex;

		// The condition expression is the next expression in the AST buffer.
	};

	struct WhileStatement : public BaseStatement<StatementType::WHILE> {
		uint32 scopeStatementIndex;

		// The condition expression is the next expression in the AST buffer.
	};

	struct BreakStatement : public BaseStatement<StatementType::BREAK> {};

	struct NextStatement : public BaseStatement<StatementType::NEXT> {};

	struct ReturnStatement : public BaseStatement<StatementType::RETURN> {
		// The expression is the next expression in the AST buffer.
	};
}