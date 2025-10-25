#pragma once

#include "CompilationOutput.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace ProjectA {
	class SemanticAnalyzer {
		public:
			void setCompilationOutput(CompilationOutput* compilationOutput);

			void analyzeStructureStatement(const ref<StructureStatement>& statement) const;
			void analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& memberStatement) const;
			void analyzeConstantStatement(const ref<ConstantStatement>& statement) const;
			void analyzeFunctionStatement(uint32 statementIndex) const;
			void analyzeVariableStatement(const ref<VariableStatement>& statement) const;

			void analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression) const;
			void analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression) const;
			void analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression) const;

		private:
			void analyzeTypeExpression(uint32 referenceIndex) const;

			void analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const;
			void analyzeBindingAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzeConstantAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzeInstanceIndexAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzeLocationAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzePositionAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzeSetAnnotation(const AnnotationBlock& annotationBlock) const;
			void analyzeVertexIndexAnnotation(const AnnotationBlock& annotationBlock) const;

			bool isLanguageFunction(const Token& token) const;

		private:
			CompilationOutput* output;
	};

	/* uint32 getTypeScalarCount(Type type) const {
		DEBUG_ASSERT(type >= Type::VEC2);

		const uint32 typeIndex = static_cast<uint32>(type) - 4;

		return typeIndex / 3 + typeIndex % 3;
	} */
}