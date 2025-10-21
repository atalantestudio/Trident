#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "SemanticAnalyzer.hpp"

namespace ProjectA {
	void SemanticAnalyzer::analyze(CompilationResult& compilationResult, const AST& ast) {
		this->compilationResult = &compilationResult;
		this->ast = &ast;

		for (const uint32 globalScopeStatementIndex : this->ast->globalScopeStatementIndices) {
			const ref<Statement>& globalScopeStatement = this->ast->statements[globalScopeStatementIndex];

			analyzeStatement(globalScopeStatement);
		}
	}

	void SemanticAnalyzer::analyzeStatement(const ref<Statement>& statement) {
		switch (statement->statementType) {
			case StatementType::STRUCTURE:
				return analyzeStructureStatement(std::static_pointer_cast<StructureStatement>(statement));
			case StatementType::VARIABLE:
				return analyzeVariableStatement(std::static_pointer_cast<VariableStatement>(statement));
			case StatementType::FUNCTION:
				return analyzeFunctionStatement(std::static_pointer_cast<FunctionStatement>(statement));
			case StatementType::EXPRESSION:
				return analyzeExpressionStatement(std::static_pointer_cast<ExpressionStatement>(statement));
			default:
				// TODO
				break;
		}
	}

	void SemanticAnalyzer::analyzeStructureStatement(const ref<StructureStatement> statement) {
		analyzeIdentifier(statement->identifierTokenIndex);

		const Token& identifierToken = compilationResult->tokens[statement->identifierTokenIndex];

		// WARNING: Structure is empty.
		if (statement->memberStatementIndices.empty()) {
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::WARNING,
				"Structure is empty.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		}

		Annotation combinedAnnotationFlag = static_cast<Annotation>(0);

		for (uint32 memberStatementIndex = 0; memberStatementIndex < statement->memberStatementIndices.size(); memberStatementIndex += 1) {
			const ref<VariableStatement> memberStatement = std::static_pointer_cast<VariableStatement>(ast->statements[memberStatementIndex]);
			const Annotation combinedMemberAnnotationFlag = analyzeMemberStatement(memberStatement);

			if (combinedMemberAnnotationFlag & Annotation::POSITION && combinedAnnotationFlag & Annotation::POSITION) {
				// ERROR: Multiple members are annotated with "position".
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Multiple members are annotated with \"" + getAnnotationString(Annotation::POSITION) + "\".",
					identifierToken.start,
					identifierToken.getEnd()
				);
			}

			combinedAnnotationFlag |= combinedMemberAnnotationFlag;
		}
	}

	void SemanticAnalyzer::analyzeVariableStatement(const ref<VariableStatement> statement) {
		analyzeTypeExpression(statement->typeExpressionIndex);
		analyzeIdentifier(statement->identifierTokenIndex);

		const Token& identifierToken = compilationResult->tokens[statement->identifierTokenIndex];

		if (isScreamingSnakeCaseWord(identifierToken)) {
			if (statement->initializerExpressionIndex == UNDEFINED_INDEX) {
				// ERROR: Constant is not initialized.
				compilationResult->registerDiagnostic(
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

		const Token& identifierToken = compilationResult->tokens[statement->identifierTokenIndex];
		const ref<ScopeStatement> scopeStatement = std::static_pointer_cast<ScopeStatement>(ast->statements[statement->scopeStatementIndex]);

		// ERROR: Function has no return statement.
		if (scopeStatement->statementIndices.empty()) {
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::ERROR,
				"Function " + getTokenString(identifierToken) + " has no return statement.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		}

		const ref<Statement> lastStatement = ast->statements[scopeStatement->statementIndices.back()];

		// ERROR: The last statement of a function body must be a return statement.
		if (lastStatement->statementType != StatementType::RETURN) {
			compilationResult->registerDiagnostic(
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
			const ref<Statement>& statement = ast->statements[statementIndex];

			analyzeStatement(statement);
		}
	}

	Annotation SemanticAnalyzer::analyzeMemberStatement(const ref<VariableStatement> statement) {
		const Annotation combinedAnnotationFlag = analyzeAnnotationInstances(statement);

		analyzeTypeExpression(statement->typeExpressionIndex);

		// const Token& typeToken = compilationResult->tokens[statement->typeExpressionTokenIndex];
		// const Token& identifierToken = compilationResult->tokens[statement->identifierTokenIndex];

		/* if (!isLanguageType(typeToken)) {
			// ERROR: Type "type" is not a language type.
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::ERROR,
				"Type " + getTokenString(typeToken) + " is not a language type.",
				typeToken.start,
				typeToken.getEnd()
			);
		}
		else {
			compilationResult->registerSemanticToken(statement->typeTokenIndex, SemanticTokenType::TYPE, 0);
		}

		if (statement->templateArgumentTokenIndex != ~0) {
			const Token& templateArgumentToken = compilationResult->tokens[statement->templateArgumentTokenIndex];

			if (!isLanguageType(templateArgumentToken)) {
				// ERROR: Template argument type "templateArgument" is not a language type.
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Template argument type " + getTokenString(typeToken) + " is not a language type.",
					typeToken.start,
					templateArgumentToken.getEnd()
				);
			}
			else {
				compilationResult->registerSemanticToken(statement->templateArgumentTokenIndex, SemanticTokenType::TYPE, 0);
			}

			if ((isVectorType(typeToken) || isMatrixType(typeToken)) && !isScalarType(templateArgumentToken)) {
				// ERROR: "templateArgument" is not a valid "type" template argument.
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					getTokenString(templateArgumentToken) + " is not a valid " + getTokenString(typeToken) + " template argument.",
					templateArgumentToken.start,
					templateArgumentToken.getEnd()
				);
			}
		}

		if ((isVectorType(typeToken) || isMatrixType(typeToken)) && statement->templateArgumentTokenIndex == UNDEFINED_INDEX) {
			// ERROR: Type "type" requires a template argument but was not provided with one.
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::ERROR,
				"Type " + getTokenString(typeToken) + " requires a template argument but was not provided with one.",
				typeToken.start,
				typeToken.getEnd()
			);
		}

		// ERROR: "identifier" is a reserved word.
		if (isReservedWord(identifierToken)) {
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::ERROR,
				getTokenString(identifierToken) + " is a reserved word.",
				identifierToken.start,
				identifierToken.getEnd()
			);
		} */

		// TODO
		// ERROR: The identifier must not be written in SCREAMING_SNAKE_CASE.

		return combinedAnnotationFlag;
	}

	void SemanticAnalyzer::analyzeTypeExpression(uint32 typeExpressionIndex) {
		const ref<TypeExpression> typeExpression = std::static_pointer_cast<TypeExpression>(ast->expressions[typeExpressionIndex]);
		const Token& typeToken = compilationResult->tokens[typeExpression->typeTokenIndex];

		if (!isReservedWord(typeToken)) {
			// ERROR: Cannot use template arguments on user-defined structure.
			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot use template arguments on user-defined structure.",
					typeToken.start,
					typeToken.getEnd()
				);

				return;
			}

			compilationResult->registerSemanticToken(typeExpression->typeTokenIndex, SemanticTokenType::STRUCTURE, 0);

			return;
		}

		compilationResult->registerSemanticToken(typeExpression->typeTokenIndex, SemanticTokenType::KEYWORD, 0);

		// Scalar type (float, sint, uint)
		if (isScalarType(typeToken)) {
			// ERROR: Scalar type "type" does not accept template arguments.
			if (typeExpression->templateArgumentTokenIndex != UNDEFINED_INDEX) {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Scalar type " + getTokenString(typeToken) + " does not accept template arguments.",
					typeToken.start,
					typeToken.getEnd()
				);
			}
		}

		// Vector type (with float/sint/uint template argument)
		if (isVectorType(typeToken)) {
			const Token& templateArgumentToken = compilationResult->tokens[typeExpression->templateArgumentTokenIndex];

			// ERROR: "templateArgumentType" cannot be used as a template argument for type "type".
			if (!isScalarType(templateArgumentToken)) {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					getTokenString(templateArgumentToken) + " cannot be used as a template argument for type " + getTokenString(typeToken) + ".",
					templateArgumentToken.start,
					templateArgumentToken.getEnd()
				);

				return;
			}
		}

		// - Matrix type (with float32/float64 template argument)
		if (isMatrixType(typeToken)) {
			const Token& templateArgumentToken = compilationResult->tokens[typeExpression->templateArgumentTokenIndex];

			// ERROR: "templateArgumentType" cannot be used as a template argument for type "type".
			if (!isFloatingPointScalarType(templateArgumentToken)) {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					getTokenString(templateArgumentToken) + " cannot be used as a template argument for type " + getTokenString(typeToken) + ".",
					templateArgumentToken.start,
					templateArgumentToken.getEnd()
				);

				return;
			}
		}

		compilationResult->registerSemanticToken(typeExpression->templateArgumentTokenIndex, SemanticTokenType::KEYWORD, 0);
	}

	void SemanticAnalyzer::analyzeExpression(uint32 expressionIndex) {
		const ref<Expression>& expression = ast->expressions[expressionIndex];

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
		compilationResult->registerSemanticToken(expression->numberTokenIndex, SemanticTokenType::NUMBER, 0);
	}

	void SemanticAnalyzer::analyzeIdentifierExpression(const ref<IdentifierExpression> expression) {
		// TODO: Check if the identifier is defined in the current scope.
		// TODO: Select correct semantic token type.
		// compilationResult->registerSemanticToken(expression->identifierTokenIndex, SemanticTokenType::VARIABLE);
	}

	void SemanticAnalyzer::analyzeUnaryExpression(const ref<UnaryExpression> expression) {
		compilationResult->registerSemanticToken(expression->operatorTokenIndex, SemanticTokenType::OPERATOR, 0);

		analyzeExpression(expression->rightExpressionIndex);
	}

	void SemanticAnalyzer::analyzeBinaryExpression(const ref<BinaryExpression> expression) {
		analyzeExpression(expression->leftExpressionIndex);

		compilationResult->registerSemanticToken(expression->operatorTokenIndex, SemanticTokenType::OPERATOR, 0);

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
		const Token& identifierToken = compilationResult->tokens[identifierTokenIndex];

		if (isReservedWord(identifierToken)) {
			// ERROR: Identifier "identifier" is reserved.
			compilationResult->registerDiagnostic(
				DiagnosticSeverity::ERROR,
				"Identifier " + getTokenString(identifierToken) + " is reserved.",
				identifierToken.start,
				identifierToken.getEnd()
			);

			compilationResult->registerSemanticToken(identifierTokenIndex, SemanticTokenType::KEYWORD, 0);

			return;
		}

		if (isScreamingSnakeCaseWord(identifierToken)) {
			compilationResult->registerSemanticToken(identifierTokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

			return;
		}

		if (isPascalCaseWord(identifierToken)) {
			compilationResult->registerSemanticToken(identifierTokenIndex, SemanticTokenType::STRUCTURE, 0);

			return;
		}

		compilationResult->registerSemanticToken(identifierTokenIndex, SemanticTokenType::VARIABLE, 0);
	}

	Annotation SemanticAnalyzer::analyzeAnnotationInstances(const ref<VariableStatement>& statement) {
		Annotation combinedAnnotationFlag = static_cast<Annotation>(0);

		/* for (uint32 annotationInstanceIndex = statement->annotationInstanceIndexOffset; annotationInstanceIndex < statement->annotationInstanceCount; annotationInstanceIndex += 1) {
			const AnnotationInstance& annotationInstance = ast->annotationInstances[annotationInstanceIndex];

			if (combinedAnnotationFlag & annotationInstance.annotation) {
				// ERROR: Duplicate instance of annotation "annotation".
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Duplicate instance of annotation \"" + getAnnotationString(annotationInstance.annotation) + "\".",
					annotationInstance.identifier.start,
					annotationInstance.identifier.getEnd()
				);

				// This annotation has already been validated on this member.
				continue;
			}

			combinedAnnotationFlag |= annotationInstance.annotation;

			if (!annotationInstance.value.value.empty()) {
				// ERROR: Annotation "annotation" doesn't require a value but was provided with one.
				if (ANNOTATION_TYPE_VOID & annotationInstance.annotation) {
					compilationResult->registerDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationInstance.annotation) + "\" doesn't require a value but was provided with one.",
						annotationInstance.value.start,
						annotationInstance.value.getEnd()
					);
				}

				if (ANNOTATION_TYPE_UINT32 & annotationInstance.annotation && (annotationInstance.value.type < TokenType::LITERAL_FLOATING_POINT || annotationInstance.value.type > TokenType::LITERAL_INTEGER_BASE_16)) {
					// ERROR: Annotation "annotation" requires a uint32 value but was not provided with an integer value.
					compilationResult->registerDiagnostic(
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
					compilationResult->registerDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationInstance.annotation) + "\" requires a uint32 value but was not provided with one.",
						annotationInstance.identifier.start,
						annotationInstance.identifier.getEnd()
					);
				}
			}
		}

		const Token& typeToken = compilationResult->tokens[statement->typeTokenIndex];
		const Token& templateArgumentToken = compilationResult->tokens[statement->templateArgumentTokenIndex];

		if (combinedAnnotationFlag & Annotation::POSITION && (typeToken.value != "vec4" || templateArgumentToken.value != "float32")) {
			// ERROR: Cannot annotate member of type "type" with annotation "position".
			if (!templateArgumentToken.value.empty()) {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot annotate member of type \"" + typeToken.value + "<" + getTokenString(templateArgumentToken) + ">\" with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					templateArgumentToken.getEnd()
				);
			}
			else {
				compilationResult->registerDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot annotate member of type " + getTokenString(typeToken) + " with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					typeToken.getEnd()
				);
			}
		} */

		return combinedAnnotationFlag;
	}

	bool SemanticAnalyzer::isLanguageType(const Token& identifier) const {
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
		for (const std::string& type : ast->userDefinedTypes) {
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
	}
}