#pragma once

#include "CompilationOutput.hpp"
#include "Diagnostic/DiagnosticCode.hpp"
#include "ExpressionEvaluation.hpp"
#include "OperatorType.hpp"
#include "Statement.hpp"

namespace ProjectA {
	class SemanticAnalyzer {
		public:
			explicit SemanticAnalyzer(CompilationOutput& output);

		private:
			DiagnosticCode analyzeStatement();
			DiagnosticCode analyzeStructureStatement(const Statement<StatementType::STRUCTURE>& statement);
			DiagnosticCode analyzeMemberStatement(const Statement<StatementType::MEMBER>& statement);
			DiagnosticCode analyzeConstantStatement(const Statement<StatementType::CONSTANT>& statement);
			DiagnosticCode analyzeVariableStatement(const Statement<StatementType::VARIABLE>& statement);
			DiagnosticCode analyzeParameterStatement(const Statement<StatementType::PARAMETER>& statement);
			DiagnosticCode analyzeFunctionStatement(const Statement<StatementType::FUNCTION>& statement);
			DiagnosticCode analyzeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement);
			DiagnosticCode analyzeScopeStatement(const Statement<StatementType::SCOPE>& statement);
			DiagnosticCode analyzeIfStatement(const Statement<StatementType::IF>& statement);
			DiagnosticCode analyzeWhileStatement(const Statement<StatementType::WHILE>& statement);
			DiagnosticCode analyzeBreakStatement(const Statement<StatementType::BREAK>& statement);
			DiagnosticCode analyzeNextStatement(const Statement<StatementType::NEXT>& statement);
			DiagnosticCode analyzeReturnStatement(const Statement<StatementType::RETURN>& statement);

			DiagnosticCode analyzeExpression();
			DiagnosticCode analyzeLiteralExpression();
			DiagnosticCode analyzeIdentifierExpression();
			DiagnosticCode analyzeUnaryExpression();
			DiagnosticCode analyzeBinaryExpression();
			DiagnosticCode analyzeGroupExpression();
			DiagnosticCode analyzeMemberAccessExpression() const;
			DiagnosticCode analyzeCallExpression();
			DiagnosticCode analyzeTypeExpression() const;

			void analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount);

			sint64 applyUnaryOperator(sint64 rvalue, OperatorType operatorType) const;
			sint64 applyBinaryOperator(sint64 lvalue, sint64 rvalue, OperatorType operatorType) const;

		private:
			CompilationOutput* output;

			uint32 statementIndex;
			uint32 statementByteOffset;
			uint32 expressionReferenceIndex;
			uint32 expressionByteOffset;
			uint32 memberDefinitionOffset;
			DocumentPosition position;

			ExpressionEvaluation evaluation;
	};
}