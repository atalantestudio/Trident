#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Diagnostic/Warning.hpp"
#include "SemanticAnalyzer.hpp"

namespace ProjectA {
	void SemanticAnalyzer::setCompilationOutput(CompilationOutput* compilationOutput) {
		this->output = compilationOutput;
	}

	void SemanticAnalyzer::analyzeStructureStatement(const ref<StructureStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isPascalCase(identifierToken.value, identifierToken.characterCount)) {
			generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(*output, identifierToken);

			return;
		}

		// TODO: Structure statement index.
		output->define(statement->identifierTokenIndex, 0);

		if (statement->memberStatementIndices.empty()) {
			generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE>(*output, identifierToken);
		}

		/* Annotation combinedAnnotationFlag = static_cast<Annotation>(0);

		for (uint32 memberStatementIndex = 0; memberStatementIndex < statement->memberStatementIndices.size(); memberStatementIndex += 1) {
			const ref<VariableStatement> memberStatement = std::static_pointer_cast<VariableStatement>(output->ast.statements[memberStatementIndex]);
			const Annotation combinedMemberAnnotationFlag = analyzeMemberStatement(memberStatement);

			if (combinedMemberAnnotationFlag & Annotation::POSITION && combinedAnnotationFlag & Annotation::POSITION) {
				// ERROR: Multiple members are annotated with "position".
				output->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Multiple members are annotated with \"" + getAnnotationString(Annotation::POSITION) + "\".",
					identifierToken.start,
					identifierToken.getEnd()
				);
			}

			combinedAnnotationFlag |= combinedMemberAnnotationFlag;
		} */
	}

	void SemanticAnalyzer::analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<StructureMemberStatement>& memberStatement) const {
		analyzeAnnotationInstances(memberStatement->annotationInstanceIndexOffset, memberStatement->annotationInstanceCount);
		analyzeTypeExpression(memberStatement->typeExpressionIndex);

		const Token& identifierToken = output->tokens[memberStatement->identifierTokenIndex];
		const std::string identifierTokenString = getTokenString(identifierToken);

		for (const uint32 memberIdentifierTokenIndex : structureStatement->memberIdentifierTokenIndices) {
			const Token& definitionToken = output->tokens[memberIdentifierTokenIndex];

			if (identifierTokenString == getTokenString(definitionToken)) {
				generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, definitionToken, identifierToken);

				break;
			}
		}

		if (!isCamelCaseOrSnakeCase(identifierToken.value, identifierToken.characterCount)) {
			generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE>(*output, identifierToken);
		}

		const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(output->ast.expressions[memberStatement->typeExpressionIndex]);
		const Token& typeToken = output->tokens[typeExpression->typeTokenIndex];

		const Token& structureIdentifierToken = output->tokens[structureStatement->identifierTokenIndex];

		if (getTokenString(typeToken) == getTokenString(structureIdentifierToken)) {
			generateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE>(*output, typeToken);
		}

		output->ast.userDefinedTypes.emplace_back(getTokenString(structureIdentifierToken));
	}

	void SemanticAnalyzer::analyzeGlobalScopeConstantStatement(const ref<GlobalScopeConstantStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isScreamingSnakeCase(identifierToken.value, identifierToken.characterCount)) {
			generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE>(*output, identifierToken);

			return;
		}

		output->define(statement->identifierTokenIndex);
	}

	void SemanticAnalyzer::analyzeFunctionStatement(uint32 statementIndex) const {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isCamelCaseOrSnakeCase(identifierToken.value, identifierToken.characterCount)) {
			generateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE>(*output, identifierToken);

			return;
		}

		output->define(statement->identifierTokenIndex);

		if (statementIndex == 0) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);

			return;
		}

		const uint32 previousStatementIndex = statementIndex - 1;
		const ref<Statement> previousStatement = output->ast.statements[previousStatementIndex];

		if (previousStatement->statementType != StatementType::RETURN) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeVariableStatement(const ref<VariableStatement>& statement) const {
		analyzeTypeExpression(statement->typeExpressionIndex);
	}

	void SemanticAnalyzer::analyzeTypeExpression(uint32 typeExpressionIndex) const {
		const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(output->ast.expressions[typeExpressionIndex]);
		const Token& typeToken = output->tokens[typeExpression->typeTokenIndex];

		if (typeToken.type < TokenType::TYPE_FLOAT32) {
			// TODO: Generate diagnostic for this case?
			return;
		}

		if (typeToken.type >= TokenType::USER_DEFINED) {
			bool undefined = true;

			for (const std::string& userDefinedType : output->ast.userDefinedTypes) {
				if (getTokenString(typeToken) == userDefinedType) {
					undefined = false;

					break;
				}
			}

			if (undefined) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(*output, typeToken);
			}

			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);
			}

			return;
		}

		if (typeToken.type <= TokenType::TYPE_UINT32 || typeToken.type >= TokenType::USER_DEFINED) {
			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);
			}

			return;
		}

		if (typeToken.type <= TokenType::TYPE_VEC4) {
			if (typeExpression->templateArgumentTokenIndex == UNDEFINED_INDEX) {
				generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(*output, typeToken);

				return;
			}

			const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

			if (templateArgumentToken.type < TokenType::TYPE_FLOAT32 || templateArgumentToken.type > TokenType::TYPE_UINT32) {
				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);
			}

			return;
		}

		if (typeToken.type <= TokenType::TYPE_MAT4X4) {
			if (typeExpression->templateArgumentTokenIndex == UNDEFINED_INDEX) {
				generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(*output, typeToken);

				return;
			}

			const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

			if (templateArgumentToken.type < TokenType::TYPE_FLOAT32 || templateArgumentToken.type > TokenType::TYPE_FLOAT64) {
				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);
			}

			return;
		}
	}

	void SemanticAnalyzer::analyzeIdentifierExpression(const ref<IdentifierExpression>& expression) const {
		const Token& identifierToken = output->tokens[expression->identifierTokenIndex];

		if (!output->isDefined(identifierToken) && !isLanguageFunction(identifierToken)) {
			generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeMemberAccessExpression(const ref<MemberAccessExpression>& expression) const {
		// The left expression has already been analyzed; only check the identifier.

		const Token& identifierToken = output->tokens[expression->identifierTokenIndex];

		if (!output->isDefined(identifierToken)) {
			generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, identifierToken);
		}
	}

	/* void SemanticAnalyzer::analyzeNumberExpression(const ref<NumberExpression> expression) {
		// TODO: Check if the number is too large.
		compilationOutput->registerSemanticToken(expression->numberTokenIndex, SemanticTokenType::NUMBER, 0);
	}

	void SemanticAnalyzer::analyzeIdentifierExpression(const ref<IdentifierExpression> expression) {
		// TODO: Select correct semantic token type.
		// compilationOutput->registerSemanticToken(expression->identifierTokenIndex, SemanticTokenType::VARIABLE);
	}

	void SemanticAnalyzer::analyzeUnaryExpression(const ref<UnaryExpression> expression) {
		compilationOutput->registerSemanticToken(expression->operatorTokenIndex, SemanticTokenType::OPERATOR, 0);

		analyzeExpression(expression->rightExpressionIndex);
	}

	void SemanticAnalyzer::analyzeBinaryExpression(const ref<BinaryExpression> expression) {
		analyzeExpression(expression->leftExpressionIndex);

		compilationOutput->registerSemanticToken(expression->operatorTokenIndex, SemanticTokenType::OPERATOR, 0);

		analyzeExpression(expression->rightExpressionIndex);
	}

	void SemanticAnalyzer::analyzeGroupExpression(const ref<GroupExpression> expression) {
		analyzeExpression(expression->expressionIndex);
	}

	void SemanticAnalyzer::analyzeMemberAccessExpression(const ref<MemberAccessExpression> expression) {
		// TODO
	}

	void SemanticAnalyzer::analyzeFunctionCallExpression(const ref<FunctionCallExpression> expression) {
		// TODO
	}

	void SemanticAnalyzer::analyzeIdentifier(uint32 identifierTokenIndex) {
		const Token& identifierToken = compilationOutput->tokens[identifierTokenIndex];

		if (isReservedWord(identifierToken)) {
			// ERROR: Identifier "identifier" is reserved.
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				"Identifier " + getTokenString(identifierToken) + " is reserved.",
				identifierToken.start,
				identifierToken.getEnd()
			);

			compilationOutput->registerSemanticToken(identifierTokenIndex, SemanticTokenType::KEYWORD, 0);

			return;
		}

		if (isScreamingSnakeCaseWord(identifierToken)) {
			compilationOutput->registerSemanticToken(identifierTokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

			return;
		}

		if (isPascalCaseWord(identifierToken)) {
			compilationOutput->registerSemanticToken(identifierTokenIndex, SemanticTokenType::STRUCTURE, 0);

			return;
		}

		compilationOutput->registerSemanticToken(identifierTokenIndex, SemanticTokenType::VARIABLE, 0);
	} */

	void SemanticAnalyzer::analyzeAnnotationInstances(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount) const {
		Annotation uniqueAnnotationFlag = static_cast<Annotation>(0);

		for (uint32 annotationInstanceIndex = 0; annotationInstanceIndex < annotationInstanceCount; annotationInstanceIndex += 1) {
			const AnnotationInstance& annotationInstance = output->ast.annotationInstances[annotationInstanceIndexOffset + annotationInstanceIndex];

			if (uniqueAnnotationFlag & annotationInstance.annotation) {
				const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

				generateDiagnostic<DiagnosticCode::DUPLICATE_ANNOTATION>(*output, identifierToken);

				continue;
			}

			switch (annotationInstance.annotation) {
				case Annotation::BINDING: {
					analyzeBindingAnnotation(annotationInstance);

					break;
				}
				case Annotation::CONSTANT: {
					analyzeConstantAnnotation(annotationInstance);

					break;
				}
				case Annotation::INSTANCE_INDEX: {
					analyzeInstanceIndexAnnotation(annotationInstance);

					break;
				}
				case Annotation::LOCATION: {
					analyzeLocationAnnotation(annotationInstance);

					break;
				}
				case Annotation::POSITION: {
					analyzePositionAnnotation(annotationInstance);

					break;
				}
				case Annotation::SET: {
					analyzeSetAnnotation(annotationInstance);

					break;
				}
				case Annotation::VERTEX_INDEX: {
					analyzeVertexIndexAnnotation(annotationInstance);

					break;
				}
				default: {
					// TODO
					BREAKPOINT();

					break;
				}
			}

			uniqueAnnotationFlag |= annotationInstance.annotation;
		}

			/* if (!annotationInstance.value.value.empty()) {
				// ERROR: Annotation "annotation" doesn't require a value but was provided with one.
				if (ANNOTATION_TYPE_VOID & annotationInstance.annotation) {
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationInstance.annotation) + "\" doesn't require a value but was provided with one.",
						annotationInstance.value.start,
						annotationInstance.value.getEnd()
					);
				}

				if (ANNOTATION_TYPE_UINT32 & annotationInstance.annotation && (annotationInstance.value.type < TokenType::LITERAL_FLOATING_POINT || annotationInstance.value.type > TokenType::LITERAL_INTEGER_BASE_16)) {
					// ERROR: Annotation "annotation" requires a uint32 value but was not provided with an integer value.
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationInstance.annotation) + "\" requires a uint32 value but was not provided with an integer value.",
						annotationInstance.value.start,
						annotationInstance.value.getEnd()
					);
				}
			}
			else {
				if (ANNOTATION_TYPE_UINT32 & annotationInstance.annotation) {
					// ERROR: Annotation "annotation" requires a uint32 value but was not provided with one.
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationInstance.annotation) + "\" requires a uint32 value but was not provided with one.",
						annotationInstance.identifier.start,
						annotationInstance.identifier.getEnd()
					);
				}
			}
		}

		const Token& typeToken = compilationOutput->tokens[statement->typeTokenIndex];
		const Token& templateArgumentToken = compilationOutput->tokens[statement->templateArgumentTokenIndex];

		if (combinedAnnotationFlag & Annotation::POSITION && (typeToken.value != "vec4" || templateArgumentToken.value != "float32")) {
			// ERROR: Cannot annotate member of type "type" with annotation "position".
			if (!templateArgumentToken.value.empty()) {
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot annotate member of type \"" + typeToken.value + "<" + getTokenString(templateArgumentToken) + ">\" with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					templateArgumentToken.getEnd()
				);
			}
			else {
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot annotate member of type " + getTokenString(typeToken) + " with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					typeToken.getEnd()
				);
			}
		} */

		// return combinedAnnotationFlag;
	}

	void SemanticAnalyzer::analyzeBindingAnnotation(const AnnotationInstance& annotationInstance) const {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeConstantAnnotation(const AnnotationInstance& annotationInstance) const {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeInstanceIndexAnnotation(const AnnotationInstance& annotationInstance) const {}

	void SemanticAnalyzer::analyzeLocationAnnotation(const AnnotationInstance& annotationInstance) const {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzePositionAnnotation(const AnnotationInstance& annotationInstance) const {}

	void SemanticAnalyzer::analyzeSetAnnotation(const AnnotationInstance& annotationInstance) const {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeVertexIndexAnnotation(const AnnotationInstance& annotationInstance) const {}

	bool SemanticAnalyzer::isLanguageFunction(const Token& token) const {
		return token.type >= FIRST_FUNCTION_INDEX && token.type <= LAST_FUNCTION_INDEX;
	}
}