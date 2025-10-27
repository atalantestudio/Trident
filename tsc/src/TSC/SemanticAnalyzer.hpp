#pragma once

#include "CompilationOutput.hpp"
#include "ExpressionEvaluation.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace ProjectA {
	class SemanticAnalyzer {
		public:
			void setCompilationOutput(CompilationOutput* compilationOutput);

			void analyzeStructureStatement(const ref<StructureStatement>& statement, DocumentPosition end) const;
			void analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& memberStatement) const;
			void analyzeConstantStatement(const ref<ConstantStatement>& statement) const;
			void analyzeFunctionStatement(uint32 statementIndex, DocumentPosition end) const;
			void analyzeParameterStatement(const ref<ParameterStatement>& statement) const;
			void analyzeVariableStatement(const ref<VariableStatement>& statement) const;

			void analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression) const;
			void analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression) const;
			void analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression) const;

		private:
			void analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const;

			void evaluateExpression(uint32 referenceIndex, ExpressionEvaluation& evaluation) const;
			void evaluateLiteralExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateIdentifierExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateUnaryExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateBinaryExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateGroupExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateMemberAccessExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateFunctionCallExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;
			void evaluateTypeExpression(const ExpressionReference& reference, ExpressionEvaluation& evaluation) const;

		private:
			CompilationOutput* output;
	};
}