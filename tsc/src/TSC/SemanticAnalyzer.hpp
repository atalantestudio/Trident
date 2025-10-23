#pragma once

#include "CompilationOutput.hpp"
#include "Statement.hpp"
#include "Token.hpp"

namespace ProjectA {
	class SemanticAnalyzer {
		private:
			static constexpr uint32 UNDEFINED_INDEX = ~0;

		public:
			void setCompilationOutput(CompilationOutput* compilationOutput);

			void analyzeStructureStatement(const ref<StructureStatement>& statement) const;
			void analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<StructureMemberStatement>& memberStatement) const;
			void analyzeGlobalScopeConstantStatement(const ref<GlobalScopeConstantStatement>& statement) const;
			void analyzeFunctionStatement(uint32 statementIndex) const;
			void analyzeVariableStatement(const ref<VariableStatement>& statement) const;

			void analyzeIdentifierExpression(const ref<IdentifierExpression>& expression) const;
			void analyzeMemberAccessExpression(const ref<MemberAccessExpression>& expression) const;

		private:
			void analyzeTypeExpression(uint32 typeExpressionIndex) const;

			void analyzeAnnotationInstances(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount) const;
			void analyzeBindingAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzeConstantAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzeInstanceIndexAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzeLocationAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzePositionAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzeSetAnnotation(const AnnotationInstance& annotationInstance) const;
			void analyzeVertexIndexAnnotation(const AnnotationInstance& annotationInstance) const;

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