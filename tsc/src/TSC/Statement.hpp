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
		uint32 typeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
	};

	struct ConstantStatement : public BaseStatement<StatementType::CONSTANT> {
		uint32 annotationFlag;
		uint32 annotationBlockIndexOffset;
		uint32 annotationBlockCount;
		uint32 typeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
		uint32 initializerExpressionReferenceIndex;
	};

	struct VariableStatement : public BaseStatement<StatementType::VARIABLE> {
		uint32 typeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
		bool constant = false;
		uint32 initializerExpressionReferenceIndex = ~0;
	};

	struct ParameterStatement : public BaseStatement<StatementType::PARAMETER> {
		uint32 typeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
	};

	struct FunctionStatement : public BaseStatement<StatementType::FUNCTION> {
		uint32 returnTypeExpressionReferenceIndex;
		uint32 identifierTokenIndex;
		std::vector<uint32> parameterStatementIndices;
		uint32 scopeStatementIndex;
	};

	struct ExpressionStatement : public BaseStatement<StatementType::EXPRESSION> {
		uint32 expressionReferenceIndex;
	};

	struct ScopeStatement : public BaseStatement<StatementType::SCOPE> {
		std::vector<uint32> statementIndices;
	};

	struct IfStatement : public BaseStatement<StatementType::IF> {
		uint32 conditionExpressionReferenceIndex;
		uint32 scopeStatementIndex;
	};

	struct WhileStatement : public BaseStatement<StatementType::WHILE> {
		uint32 conditionExpressionReferenceIndex;
		uint32 scopeStatementIndex;
	};

	struct BreakStatement : public BaseStatement<StatementType::BREAK> {};

	struct NextStatement : public BaseStatement<StatementType::NEXT> {};

	struct ReturnStatement : public BaseStatement<StatementType::RETURN> {
		uint32 expressionReferenceIndex;
	};
}