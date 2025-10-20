#pragma once

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

	struct StructureStatement : public BaseStatement<StatementType::STRUCTURE> {
		uint32 identifierTokenIndex;
		std::vector<uint32> memberStatementIndices;
	};

	struct VariableStatement : public BaseStatement<StatementType::VARIABLE> {
		uint32 annotationInstanceIndexOffset;
		uint32 annotationInstanceCount;
		uint32 typeExpressionIndex;
		uint32 identifierTokenIndex;

		// TODO: Register '=' semantic token before the semantic analysis; this way, the token index will not need to be stored in the expression.
		uint32 initializerOperatorTokenIndex = ~0;

		uint32 initializerExpressionIndex = ~0;
	};

	struct FunctionStatement : public BaseStatement<StatementType::FUNCTION> {
		uint32 returnTypeExpressionIndex;
		uint32 identifierTokenIndex;
		std::vector<uint32> parameterStatementIndices;
		uint32 scopeStatementIndex;
	};

	struct ExpressionStatement : public BaseStatement<StatementType::EXPRESSION> {
		uint32 expressionIndex;
	};

	struct ScopeStatement : public BaseStatement<StatementType::SCOPE> {
		std::vector<uint32> statementIndices;
	};

	struct IfStatement : public BaseStatement<StatementType::IF> {
		uint32 conditionExpressionIndex;
		uint32 scopeStatementIndex;
	};

	struct WhileStatement : public BaseStatement<StatementType::WHILE> {
		uint32 conditionExpressionIndex;
		uint32 scopeStatementIndex;
	};

	struct ReturnStatement : public BaseStatement<StatementType::RETURN> {
		uint32 expressionIndex;
	};
}