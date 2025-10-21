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

	struct StructureMemberStatement : public BaseStatement<StatementType::STRUCTURE_MEMBER> {
		uint32 annotationFlag;
		uint32 annotationInstanceIndexOffset;
		uint32 annotationInstanceCount;
		uint32 typeExpressionIndex;
		uint32 identifierTokenIndex;
	};

	struct GlobalScopeConstantStatement : public BaseStatement<StatementType::GLOBAL_SCOPE_CONSTANT> {
		uint32 annotationFlag;
		uint32 annotationInstanceIndexOffset;
		uint32 annotationInstanceCount;
		uint32 typeExpressionIndex;
		uint32 identifierTokenIndex;
		uint32 initializerExpressionIndex;
	};

	struct VariableStatement : public BaseStatement<StatementType::VARIABLE> {
		uint32 typeExpressionIndex;
		uint32 identifierTokenIndex;
		bool constant = false;
		uint32 initializerExpressionIndex = ~0;
	};

	struct ParameterStatement : public BaseStatement<StatementType::PARAMETER> {
		uint32 typeExpressionIndex;
		uint32 identifierTokenIndex;
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