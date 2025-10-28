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
			void analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& statement) const;
			void analyzeConstantStatement(const ref<ConstantStatement>& statement) const;
			void analyzeFunctionStatement(uint32 statementIndex, DocumentPosition end) const;
			void analyzeParameterStatement(const ref<ParameterStatement>& statement) const;
			void analyzeVariableStatement(const ref<VariableStatement>& statement) const;

			void analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression) const;
			void analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression) const;
			void analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression) const;

		private:
			void analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const;

			void evaluateExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateLiteralExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateIdentifierExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateUnaryExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateBinaryExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateGroupExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateMemberAccessExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateCallExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;
			void evaluateTypeExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const;

		private:
			CompilationOutput* output;
	};
}