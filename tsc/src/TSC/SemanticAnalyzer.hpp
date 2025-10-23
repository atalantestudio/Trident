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

			void analyzeStructureStatement(const ref<StructureStatement>& statement);
			void analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<StructureMemberStatement>& memberStatement);
			// void analyze(CompilationOutput& compilationOutput, const AST& ast);

			void analyzeIdentifierExpression(const ref<IdentifierExpression>& expression) const;
			void analyzeMemberAccessExpression(const ref<MemberAccessExpression>& expression) const;

		private:
			// void analyzeStatement(const ref<Statement>& statement);
			// void analyzeVariableStatement(const ref<VariableStatement> statement);
			// void analyzeFunctionStatement(const ref<FunctionStatement> statement);
			// void analyzeExpressionStatement(const ref<ExpressionStatement> statement);
			// void analyzeScopeStatement(const ref<ScopeStatement> statement);

			// Returns a flag representing the combined annotations of the member.
			// Annotation analyzeMemberStatement(const ref<VariableStatement> statement);

			void analyzeTypeExpression(uint32 typeExpressionIndex);
			// void analyzeExpression(uint32 expressionIndex);
			// void analyzeNumberExpression(const ref<NumberExpression> expression);
			// void analyzeUnaryExpression(const ref<UnaryExpression> expression);
			// void analyzeBinaryExpression(const ref<BinaryExpression> expression);
			// void analyzeGroupExpression(const ref<GroupExpression> expression);
			// void analyzeMemberAccessExpression(const ref<MemberAccessExpression> expression);
			// void analyzeFunctionCallExpression(const ref<FunctionCallExpression> expression);

			// void analyzeIdentifier(uint32 identifierTokenIndex);

			// Returns a flag representing the combined annotations of the member.
			void analyzeAnnotationInstances(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount);
			void analyzeBindingAnnotation(const AnnotationInstance& annotationInstance);
			void analyzeConstantAnnotation(const AnnotationInstance& annotationInstance);
			void analyzeInstanceIndexAnnotation(const AnnotationInstance& annotationInstance);
			void analyzeLocationAnnotation(const AnnotationInstance& annotationInstance);
			void analyzePositionAnnotation(const AnnotationInstance& annotationInstance);
			void analyzeSetAnnotation(const AnnotationInstance& annotationInstance);
			void analyzeVertexIndexAnnotation(const AnnotationInstance& annotationInstance);

			// bool isLanguageType(const Token& identifier) const;
			// bool isScalarType(const Token& identifier) const;
			// bool isFloatingPointScalarType(const Token& identifier) const;
			// bool isVectorType(const Token& identifier) const;
			// bool isMatrixType(const Token& identifier) const;
			// bool isUserDefinedType(const Token& identifier) const;
			// bool isReservedWord(const Token& identifier) const;

			// bool isPascalCaseWord(const Token& identifier) const;
			// bool isScreamingSnakeCaseWord(const Token& identifier) const;

		private:
			CompilationOutput* output;
	};

	/* uint32 getTypeScalarCount(Type type) const {
		DEBUG_ASSERT(type >= Type::VEC2);

		const uint32 typeIndex = static_cast<uint32>(type) - 4;

		return typeIndex / 3 + typeIndex % 3;
	} */
}