#pragma once

#include "CompilationOutput.hpp"
#include "ExpressionEvaluation.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace ProjectA {
	class SemanticAnalyzer {
		public:
			void setCompilationOutput(CompilationOutput* compilationOutput);

			void analyzeStructureStatement(uint32 statementIndex, const ref<StructureStatement>& statement, DocumentPosition end, uint32 memberTypeExpressionReferenceOffset);
			void analyzeMemberStatement(uint32 structureStatementIndex, const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& statement) const;
			void analyzeConstantStatement(const ref<ConstantStatement>& statement);
			void analyzeFunctionStatement(uint32 statementIndex, DocumentPosition end) const;
			void analyzeParameterStatement(const ref<ParameterStatement>& statement) const;
			void analyzeVariableStatement(const ref<VariableStatement>& statement);

			void analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression);
			void analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression);
			void analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression);

		private:
			const AnnotationBlock& getAnnotationBlock(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount, Annotation annotation) const;

			void analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const;

			void evaluateNextExpression(ExpressionEvaluation& evaluation);
			void evaluateLiteralExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateIdentifierExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateUnaryExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateBinaryExpression(ExpressionEvaluation& leftEvaluation, uint32 byteOffset);
			void evaluateGroupExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateMemberAccessExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateCallExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);
			void evaluateTypeExpression(ExpressionEvaluation& evaluation, uint32 byteOffset);

		private:
			CompilationOutput* output;
			uint32 currentExpressionReferenceIndex;
	};
}