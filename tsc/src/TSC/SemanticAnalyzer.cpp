#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "SemanticAnalyzer.hpp"

namespace ProjectA {
	void SemanticAnalyzer::setCompilationOutput(CompilationOutput* compilationOutput) {
		this->output = compilationOutput;
	}

	void SemanticAnalyzer::analyzeStructureStatement(const ref<StructureStatement>& statement) {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isPascalCase(identifierToken.value.c_str())) {
			generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(*output, identifierToken);
		}

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

	void SemanticAnalyzer::analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<StructureMemberStatement>& memberStatement) {
		analyzeAnnotationInstances(memberStatement->annotationInstanceIndexOffset, memberStatement->annotationInstanceCount);
		analyzeTypeExpression(memberStatement->typeExpressionIndex);

		const Token& identifierToken = output->tokens[memberStatement->identifierTokenIndex];

		for (const uint32 memberIdentifierTokenIndex : structureStatement->memberIdentifierTokenIndices) {
			const Token& definitionToken = output->tokens[memberIdentifierTokenIndex];

			if (identifierToken.value == definitionToken.value) {
				generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, definitionToken, identifierToken);

				break;
			}
		}

		if (!isCamelCaseOrSnakeCase(identifierToken.value.c_str())) {
			generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE>(*output, identifierToken);
		}

		const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(output->ast.expressions[memberStatement->typeExpressionIndex]);
		const Token& typeToken = output->tokens[typeExpression->typeTokenIndex];

		const Token& structureIdentifierToken = output->tokens[structureStatement->identifierTokenIndex];

		if (typeToken.value == structureIdentifierToken.value) {
			generateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE>(*output, typeToken);
		}

		output->ast.userDefinedTypes.emplace_back(structureIdentifierToken.value);
	}

	/* void SemanticAnalyzer::analyzeVariableStatement(const ref<VariableStatement> statement) {
		analyzeTypeExpression(statement->typeExpressionIndex);
		analyzeIdentifier(statement->identifierTokenIndex);

		const Token& identifierToken = compilationOutput->tokens[statement->identifierTokenIndex];

		if (isScreamingSnakeCaseWord(identifierToken)) {
			if (statement->initializerExpressionIndex == UNDEFINED_INDEX) {
				// ERROR: Constant is not initialized.
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Constant is not initialized.",
					identifierToken.start,
					identifierToken.getEnd()
				);

				return;
			}

			analyzeExpression(statement->initializerExpressionIndex);
		}
	}

	void SemanticAnalyzer::analyzeFunctionStatement(const ref<FunctionStatement> statement) {
		analyzeTypeExpression(statement->returnTypeExpressionIndex);

		const Token& identifierToken = compilationOutput->tokens[statement->identifierTokenIndex];
		const ref<ScopeStatement> scopeStatement = std::static_pointer_cast<ScopeStatement>(output->ast.statements[statement->scopeStatementIndex]);

		// ERROR: Function has no return statement.
		if (scopeStatement->statementIndices.empty()) {
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				"Function " + getTokenString(identifierToken) + " has no return statement.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		}

		const ref<Statement> lastStatement = ast.statements[scopeStatement->statementIndices.back()];

		// ERROR: The last statement of a function body must be a return statement.
		if (lastStatement->statementType != StatementType::RETURN) {
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				"The last statement of a function body must be a return statement.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		}

		analyzeScopeStatement(scopeStatement);
	}

	void SemanticAnalyzer::analyzeExpressionStatement(const ref<ExpressionStatement> statement) {
		analyzeExpression(statement->expressionIndex);
	}

	void SemanticAnalyzer::analyzeScopeStatement(const ref<ScopeStatement> statement) {
		for (const uint32 statementIndex : statement->statementIndices) {
			const ref<Statement>& statement = ast.statements[statementIndex];

			analyzeStatement(statement);
		}
	} */

	/* Annotation SemanticAnalyzer::analyzeMemberStatement(const ref<VariableStatement> statement) {
		// analyzeTypeExpression(statement->typeExpressionIndex);

		// const Token& typeToken = compilationOutput->tokens[statement->typeExpressionTokenIndex];
		// const Token& identifierToken = compilationOutput->tokens[statement->identifierTokenIndex];

		if (!isLanguageType(typeToken)) {
			// ERROR: Type "type" is not a language type.
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				"Type " + getTokenString(typeToken) + " is not a language type.",
				typeToken.start,
				typeToken.getEnd()
			);
		}
		else {
			compilationOutput->registerSemanticToken(statement->typeTokenIndex, SemanticTokenType::TYPE, 0);
		}

		if (statement->templateArgumentTokenIndex != ~0) {
			const Token& templateArgumentToken = compilationOutput->tokens[statement->templateArgumentTokenIndex];

			if (!isLanguageType(templateArgumentToken)) {
				// ERROR: Template argument type "templateArgument" is not a language type.
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Template argument type " + getTokenString(typeToken) + " is not a language type.",
					typeToken.start,
					templateArgumentToken.getEnd()
				);
			}
			else {
				compilationOutput->registerSemanticToken(statement->templateArgumentTokenIndex, SemanticTokenType::TYPE, 0);
			}

			if ((isVectorType(typeToken) || isMatrixType(typeToken)) && !isScalarType(templateArgumentToken)) {
				// ERROR: "templateArgument" is not a valid "type" template argument.
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					getTokenString(templateArgumentToken) + " is not a valid " + getTokenString(typeToken) + " template argument.",
					templateArgumentToken.start,
					templateArgumentToken.getEnd()
				);
			}
		}

		if ((isVectorType(typeToken) || isMatrixType(typeToken)) && statement->templateArgumentTokenIndex == UNDEFINED_INDEX) {
			// ERROR: Type "type" requires a template argument but was not provided with one.
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				"Type " + getTokenString(typeToken) + " requires a template argument but was not provided with one.",
				typeToken.start,
				typeToken.getEnd()
			);
		}

		// ERROR: "identifier" is a reserved word.
		if (isReservedWord(identifierToken)) {
			compilationOutput->generateDiagnostic(
				DiagnosticSeverity::ERROR,
				getTokenString(identifierToken) + " is a reserved word.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		}

		// TODO
		// ERROR: The identifier must not be written in SCREAMING_SNAKE_CASE.
	} */

	void SemanticAnalyzer::analyzeTypeExpression(uint32 typeExpressionIndex) {
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
			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

				if (templateArgumentToken.type < TokenType::TYPE_FLOAT32 || templateArgumentToken.type > TokenType::TYPE_UINT32) {
					generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);

					return;
				}
			}
		}

		if (typeToken.type <= TokenType::TYPE_MAT4X4) {
			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				const Token& templateArgumentToken = output->tokens[typeExpression->templateArgumentTokenIndex];

				if (templateArgumentToken.type < TokenType::TYPE_FLOAT32 || templateArgumentToken.type > TokenType::TYPE_FLOAT64) {
					generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, typeToken, templateArgumentToken);

					return;
				}
			}
		}
	}

	/* void SemanticAnalyzer::analyzeExpression(uint32 expressionIndex) {
		const ref<Expression>& expression = ast.expressions[expressionIndex];

		switch (expression->type) {
			case ExpressionType::NUMBER:
				return analyzeNumberExpression(std::static_pointer_cast<NumberExpression>(expression));
			case ExpressionType::IDENTIFIER:
				return analyzeIdentifierExpression(std::static_pointer_cast<IdentifierExpression>(expression));
			case ExpressionType::UNARY:
				return analyzeUnaryExpression(std::static_pointer_cast<UnaryExpression>(expression));
			case ExpressionType::BINARY:
				return analyzeBinaryExpression(std::static_pointer_cast<BinaryExpression>(expression));
			case ExpressionType::GROUP:
				return analyzeGroupExpression(std::static_pointer_cast<GroupExpression>(expression));
			case ExpressionType::MEMBER_ACCESS:
				return analyzeMemberAccessExpression(std::static_pointer_cast<MemberAccessExpression>(expression));
			case ExpressionType::FUNCTION_CALL:
				return analyzeFunctionCallExpression(std::static_pointer_cast<FunctionCallExpression>(expression));
			case ExpressionType::TYPE:
				// TODO

				break;
		}
	}

	void SemanticAnalyzer::analyzeNumberExpression(const ref<NumberExpression> expression) {
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

	void SemanticAnalyzer::analyzeAnnotationInstances(uint32 annotationInstanceIndexOffset, uint32 annotationInstanceCount) {
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

	void SemanticAnalyzer::analyzeBindingAnnotation(const AnnotationInstance& annotationInstance) {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeConstantAnnotation(const AnnotationInstance& annotationInstance) {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeInstanceIndexAnnotation(const AnnotationInstance& annotationInstance) {}

	void SemanticAnalyzer::analyzeLocationAnnotation(const AnnotationInstance& annotationInstance) {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzePositionAnnotation(const AnnotationInstance& annotationInstance) {}

	void SemanticAnalyzer::analyzeSetAnnotation(const AnnotationInstance& annotationInstance) {
		if (annotationInstance.valueTokenIndex == ~0) {
			const Token& identifierToken = output->tokens[annotationInstance.identifierTokenIndex];

			generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeVertexIndexAnnotation(const AnnotationInstance& annotationInstance) {}

	/* bool SemanticAnalyzer::isLanguageType(const Token& identifier) const {
		return isScalarType(identifier) || isVectorType(identifier) || isMatrixType(identifier);
	}

	bool SemanticAnalyzer::isScalarType(const Token& identifier) const {
		return isFloatingPointScalarType(identifier) || identifier.value == "sint32" || identifier.value == "uint32";
	}

	bool SemanticAnalyzer::isFloatingPointScalarType(const Token& identifier) const {
		return identifier.value == "float32" || identifier.value == "float64";
	}

	bool SemanticAnalyzer::isVectorType(const Token& identifier) const {
		return identifier.value == "vec2" || identifier.value == "vec3" || identifier.value == "vec4";
	}

	bool SemanticAnalyzer::isMatrixType(const Token& identifier) const {
		return (
			identifier.value == "mat2x2" ||
			identifier.value == "mat2x3" ||
			identifier.value == "mat2x4" ||
			identifier.value == "mat3x2" ||
			identifier.value == "mat3x3" ||
			identifier.value == "mat3x4" ||
			identifier.value == "mat4x2" ||
			identifier.value == "mat4x3" ||
			identifier.value == "mat4x4"
		);
	}

	bool SemanticAnalyzer::isUserDefinedType(const Token& identifier) const {
		for (const std::string& type : ast.userDefinedTypes) {
			if (identifier.value == type) {
				return true;
			}
		}

		return false;
	}

	bool SemanticAnalyzer::isReservedWord(const Token& identifier) const {
		return isLanguageType(identifier);
	}

	bool SemanticAnalyzer::isPascalCaseWord(const Token& identifier) const {
		if (identifier.value[0] < 'A' || identifier.value[0] > 'Z') {
			return false;
		}

		for (const char8 character : identifier.value) {
			if (character == '_') {
				return false;
			}
		}

		return true;
	}

	bool SemanticAnalyzer::isScreamingSnakeCaseWord(const Token& identifier) const {
		for (const char8 character : identifier.value) {
			if ((character < 'A' || character > 'Z') && character != '_') {
				return false;
			}
		}

		return true;
	} */
}