#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Diagnostic/Warning.hpp"
#include "Language/BaseType.hpp"
#include "SemanticAnalyzer.hpp"

namespace ProjectA {
	// TODO: Move inside Base directory.
	// https://www.w3.org/TR/WGSL/#roundup
	static uint32 roundUp(uint32 k, uint32 n) {
		if (k == 0) {
			return 0;
		}

		return (static_cast<uint32>(ceil(static_cast<float32>(n) / k))) * k;
	}

	void SemanticAnalyzer::setCompilationOutput(CompilationOutput* compilationOutput) {
		this->output = compilationOutput;
	}

	void SemanticAnalyzer::analyzeStructureStatement(const ref<StructureStatement>& statement, DocumentPosition end) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isPascalCase(output->getIdentifier(identifierToken.type))) {
			generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		uint32 size = 0;
		uint32 alignment = 0;
		uint32 lastMemberOffset = 0;
		uint32 lastMemberSize = 0;

		if (statement->memberStatementIndices.empty()) {
			generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE>(*output, identifierToken);
		}

		for (const uint32 statementIndex : statement->memberStatementIndices) {
			const ref<MemberStatement>& statement = std::static_pointer_cast<MemberStatement>(output->ast.statements[statementIndex]);
			const ExpressionReference& expressionReference = output->ast.getExpressionReference(statement->typeExpressionReferenceIndex);
			const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionReference.byteOffset);

			lastMemberOffset += lastMemberSize;
			lastMemberSize = output->getSize(expression.type);
			size += lastMemberSize;

			#ifdef __EMSCRIPTEN__
				alignment = alignment > output->getAlignment(expression.type) ? alignment : output->getAlignment(expression.type);
			#else
				alignment = max(alignment, output->getAlignment(expression.type));
			#endif
		}

		size = roundUp(alignment, lastMemberOffset + lastMemberSize);

		output->defineStructure(statement, end, size, alignment);

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

	void SemanticAnalyzer::analyzeMemberStatement(const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& statement) const {
		analyzeAnnotationBlocks(statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		output->registerSemanticToken(statement->identifierTokenIndex, SemanticTokenType::MEMBER, 0);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string identifierTokenString = output->getTokenString(identifierToken);

		for (const uint32 memberStatementIndex : structureStatement->memberStatementIndices) {
			const ref<MemberStatement> memberStatement = std::static_pointer_cast<MemberStatement>(output->ast.statements[memberStatementIndex]);
			const Token& definitionToken = output->tokens[memberStatement->identifierTokenIndex];

			if (identifierTokenString == output->getTokenString(definitionToken)) {
				generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, identifierToken, memberStatement->identifierTokenIndex);

				break;
			}
		}

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken.type))) {
			generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}
	}

	void SemanticAnalyzer::analyzeConstantStatement(const ref<ConstantStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isScreamingSnakeCase(output->getIdentifier(identifierToken.type))) {
			generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		output->define(statement);

		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(statement->typeExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		ExpressionEvaluation evaluation{};
		evaluation.intermediateType = typeExpression.type;
		evaluation.casting = false;
		evaluation.constant = true;
		evaluation.value = 0;

		/* TODO
		Start the eval with type = constant type.
		If different type found:
		- If converting is true, OK
		- Else, conversion error

		If constructor found:
		- diagnostic if same type
		- set type to constructor type
		- set converting to true
		*/

		uint32 referenceCount = 0;

		evaluateExpression(statement->initializerExpressionReferenceIndex, referenceCount, evaluation);

		if (evaluation.intermediateType != typeExpression.type) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeFunctionStatement(uint32 statementIndex, DocumentPosition end) const {
		const ref<FunctionStatement> statement = std::static_pointer_cast<FunctionStatement>(output->ast.statements[statementIndex]);
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string& identifier = output->getIdentifier(identifierToken.type);

		output->registerSemanticToken(statement->identifierTokenIndex, SemanticTokenType::FUNCTION, 0);

		if (!isCamelCaseOrSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		output->defineFunction(identifier, end);

		if (statementIndex == 0) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);

			return;
		}

		// TODO: Check that there is no return statement.
		const uint32 previousStatementIndex = statementIndex - 1;
		const ref<Statement> previousStatement = output->ast.statements[previousStatementIndex];

		if (previousStatement->statementType != StatementType::RETURN) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);
		}
	}

	void SemanticAnalyzer::analyzeParameterStatement(const ref<ParameterStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string& identifier = output->getIdentifier(identifierToken.type);

		output->registerSemanticToken(statement->identifierTokenIndex, SemanticTokenType::PARAMETER, 0);

		if (!isCamelCaseOrSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(statement->typeExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		output->define(typeExpression.type, SemanticTokenType::PARAMETER, 0, statement->identifierTokenIndex, identifierToken.getEnd());
		output->nextScopeDefinitionCount += 1;
	}

	void SemanticAnalyzer::analyzeVariableStatement(const ref<VariableStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken.type)) && !isScreamingSnakeCase(output->getIdentifier(identifierToken.type))) {
			generateDiagnostic<DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		const Definition* previousDefinition = output->getFirstDefinition(output->getIdentifier(identifierToken.type));

		if (previousDefinition != nullptr) {
			generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, identifierToken, previousDefinition->tokenIndex);

			return;
		}

		output->define(statement);

		if (statement->constant) {
			const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(statement->typeExpressionReferenceIndex);
			const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

			// TODO
			BREAKPOINT();

			/* ExpressionEvaluation evaluation{};
			evaluation.casting = false;
			evaluation.constant = true;
			evaluation.value = 0;

			evaluateExpression(statement->initializerExpressionReferenceIndex, evaluation);

			// TODO: Also check vector/matrix/user-defined types.
			if (typeExpression.type.typeIndex >= BaseType::FLOAT32 && typeExpression.type.typeIndex <= BaseType::UINT32) {
				if (evaluation.intermediateType != typeExpression.type) {
					generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, identifierToken);
				}
			} */
		}
	}

	void SemanticAnalyzer::analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression) const {
		if (expression.type.hasUserDefinedType()) {
			output->registerSemanticToken(expression.typeTokenIndex, SemanticTokenType::STRUCTURE, 0);

			const UserDefinedType* userDefinedType = output->getUserDefinedType(expression.type.typeIndex);

			if (userDefinedType == nullptr) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(*output, expression.typeTokenIndex);

				return;
			}

			if (expression.type.templateArgumentIndex > 0) {
				const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex, expression.templateArgumentTokenIndex);

				return;
			}

			return;
		}

		const uint16 templateArgumentMask = TYPE_TEMPLATE_ARGUMENT_MASKS[expression.type.typeIndex];

		if (expression.type.templateArgumentIndex == 0) {
			if (templateArgumentMask == 0) {
				return;
			}

			generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex);

			return;
		}

		if (templateArgumentMask & BIT(expression.type.templateArgumentIndex)) {
			return;
		}

		generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex, expression.templateArgumentTokenIndex);
	}

	void SemanticAnalyzer::analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression) const {
		const Token& token = output->tokens[expression.tokenIndex];
		const Definition* definition = output->getFirstDefinition(output->getIdentifier(token.type));

		if (definition == nullptr) {
			if (token.type < FIRST_FUNCTION_INDEX || token.type > LAST_FUNCTION_INDEX) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, token);

				return;
			}

			output->registerSemanticToken(token, SemanticTokenType::FUNCTION, SemanticTokenModifier::PREDEFINED);

			return;
		}

		output->registerSemanticToken(expression.tokenIndex, definition->semanticTokenType, definition->semanticTokenModifierFlag);
	}

	void SemanticAnalyzer::analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression) const {
		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];
		const Definition* definition = output->getFirstDefinition(output->getIdentifier(identifierToken.type));

		if (definition == nullptr) {
			generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, identifierToken);

			return;
		}
	}

	void SemanticAnalyzer::analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const {
		Annotation uniqueAnnotationFlag = static_cast<Annotation>(0);

		for (uint32 annotationBlockIndex = 0; annotationBlockIndex < annotationBlockCount; annotationBlockIndex += 1) {
			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];

			if (uniqueAnnotationFlag & annotationBlock.annotation) {
				const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

				generateDiagnostic<DiagnosticCode::DUPLICATE_ANNOTATION>(*output, identifierToken);

				continue;
			}

			switch (annotationBlock.annotation) {
				case Annotation::BINDING:
				case Annotation::CONSTANT:
				case Annotation::LOCATION:
				case Annotation::SET: {
					if (annotationBlock.valueTokenIndex == ~0) {
						const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

						generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, identifierToken);
					}

					break;
				}
				default: {
					// TODO
					BREAKPOINT();

					break;
				}
			}

			uniqueAnnotationFlag |= annotationBlock.annotation;
		}

			/* if (!annotationBlock.value.value.empty()) {
				// ERROR: Annotation "annotation" doesn't require a value but was provided with one.
				if (ANNOTATION_TYPE_VOID & annotationBlock.annotation) {
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationBlock.annotation) + "\" doesn't require a value but was provided with one.",
						annotationBlock.value.start,
						annotationBlock.value.getEnd()
					);
				}

				if (ANNOTATION_TYPE_UINT32 & annotationBlock.annotation && (annotationBlock.value.type != TokenType::LITERAL_FLOATING_POINT && annotationBlock.value.type != TokenType::LITERAL_INTEGER)) {
					// ERROR: Annotation "annotation" requires a uint32 value but was not provided with an integer value.
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationBlock.annotation) + "\" requires a uint32 value but was not provided with an integer value.",
						annotationBlock.value.start,
						annotationBlock.value.getEnd()
					);
				}
			}
			else {
				if (ANNOTATION_TYPE_UINT32 & annotationBlock.annotation) {
					// ERROR: Annotation "annotation" requires a uint32 value but was not provided with one.
					compilationOutput->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"Annotation \"" + getAnnotationString(annotationBlock.annotation) + "\" requires a uint32 value but was not provided with one.",
						annotationBlock.identifier.start,
						annotationBlock.identifier.getEnd()
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
					"Cannot annotate member of type \"" + typeToken.value + "<" + output->getTokenString(templateArgumentToken) + ">\" with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					templateArgumentToken.getEnd()
				);
			}
			else {
				compilationOutput->generateDiagnostic(
					DiagnosticSeverity::ERROR,
					"Cannot annotate member of type " + output->getTokenString(typeToken) + " with annotation \"" + getAnnotationString(Annotation::POSITION) + "\".",
					typeToken.start,
					typeToken.getEnd()
				);
			}
		} */
	}

	void SemanticAnalyzer::evaluateExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: return evaluateLiteralExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::IDENTIFIER: return evaluateIdentifierExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::UNARY: return evaluateUnaryExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::BINARY: return evaluateBinaryExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::GROUP: return evaluateGroupExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::MEMBER_ACCESS: return evaluateMemberAccessExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::CALL: return evaluateCallExpression(referenceIndex, referenceCount, evaluation);
			case ExpressionType::TYPE: return evaluateTypeExpression(referenceIndex, referenceCount, evaluation);
		}
	}

	void SemanticAnalyzer::evaluateLiteralExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(expressionReference.byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];

		evaluation.value = output->getLiteral(token.index);

		if (token.type == TokenType::LITERAL_FLOATING_POINT) {
			if (evaluation.intermediateType.typeIndex != BaseType::FLOAT32 && evaluation.intermediateType.typeIndex != BaseType::FLOAT64) {
				generateDiagnostic<DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT>(*output, token, evaluation.intermediateType.typeIndex, evaluation.intermediateType.templateArgumentIndex);

				// TODO: Stop evaluating the expression.
				return;
			}
		}

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateIdentifierExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		// TODO

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateUnaryExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(expressionReference.byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		switch (operatorToken.type) {
			case TokenType::MINUS: {
				evaluation.value = -evaluation.value;

				break;
			}
			case TokenType::EXCLAMATION: {
				evaluation.value = !evaluation.value;

				break;
			}
			case TokenType::TILDE: {
				evaluation.value = ~evaluation.value;

				break;
			}
			default: {
				DEBUG_ASSERT(false);
			}
			case TokenType::PLUS: {
				break;
			}
		}

		evaluateExpression(expression.rightExpressionReferenceIndex, referenceCount, evaluation);

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateBinaryExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(expressionReference.byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		ExpressionEvaluation rightEvaluation{};
		rightEvaluation.constant = true;
		rightEvaluation.value = 0;

		evaluateExpression(expression.leftExpressionReferenceIndex, referenceCount, evaluation);
		evaluateExpression(expression.rightExpressionReferenceIndex, referenceCount, rightEvaluation);

		switch (operatorToken.type) {
			case TokenType::EQUAL:
			case TokenType::ASTERISK_EQUAL:
			case TokenType::SLASH_EQUAL:
			case TokenType::PLUS_EQUAL:
			case TokenType::MINUS_EQUAL:
			case TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL:
			case TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL:
			case TokenType::AMPERSAND_EQUAL:
			case TokenType::CARET_EQUAL:
			case TokenType::VERTICAL_BAR_EQUAL: {
				evaluation.constant = false;

				break;
			}
			case TokenType::VERTICAL_BAR: {
				evaluation.value |= rightEvaluation.value;

				break;
			}
			case TokenType::CARET: {
				evaluation.value ^= rightEvaluation.value;

				break;
			}
			case TokenType::AMPERSAND: {
				evaluation.value &= rightEvaluation.value;

				break;
			}
			case TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: {
				evaluation.value <<= rightEvaluation.value;

				break;
			}
			case TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: {
				evaluation.value >>= rightEvaluation.value;

				break;
			}
			case TokenType::EQUAL_EQUAL: {
				evaluation.value = evaluation.value == rightEvaluation.value;

				break;
			}
			case TokenType::EXCLAMATION_EQUAL: {
				evaluation.value = evaluation.value != rightEvaluation.value;

				break;
			}
			case TokenType::LEFT_ANGLE_BRACKET: {
				evaluation.value = evaluation.value < rightEvaluation.value;

				break;
			}
			case TokenType::LEFT_ANGLE_BRACKET_EQUAL: {
				evaluation.value = evaluation.value <= rightEvaluation.value;

				break;
			}
			case TokenType::RIGHT_ANGLE_BRACKET: {
				evaluation.value = evaluation.value > rightEvaluation.value;

				break;
			}
			case TokenType::RIGHT_ANGLE_BRACKET_EQUAL: {
				evaluation.value = evaluation.value >= rightEvaluation.value;

				break;
			}
			case TokenType::ASTERISK: {
				evaluation.value *= rightEvaluation.value;

				break;
			}
			case TokenType::SLASH: {
				evaluation.value /= rightEvaluation.value;

				break;
			}
			case TokenType::PLUS: {
				evaluation.value += rightEvaluation.value;

				break;
			}
			case TokenType::MINUS: {
				evaluation.value -= rightEvaluation.value;

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateGroupExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(expressionReference.byteOffset);

		evaluateExpression(expression.expressionReferenceIndex, referenceCount, evaluation);

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateMemberAccessExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		// TODO

		referenceCount += 1;
	}

	void SemanticAnalyzer::evaluateCallExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(expressionReference.byteOffset);

		evaluateExpression(expression.calleeExpressionReferenceIndex, referenceCount, evaluation);

		uint32 argumentExpressionReferenceIndex = referenceIndex - 1;
		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			uint32 argumentExpressionReferenceOffset = 0;

			evaluateExpression(argumentExpressionReferenceIndex, argumentExpressionReferenceOffset, evaluation);

			argumentExpressionReferenceIndex -= argumentExpressionReferenceOffset;
			argumentIndex += 1;
		}
	}

	void SemanticAnalyzer::evaluateTypeExpression(uint32 referenceIndex, uint32& referenceCount, ExpressionEvaluation& evaluation) const {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(referenceIndex);
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionReference.byteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		if (!evaluation.casting && expression.type != evaluation.intermediateType) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, typeToken);

			// TODO: Stop evaluation.
			return;
		}

		evaluation.intermediateType = expression.type;
		evaluation.casting = true;

		referenceCount += 1;
	}
}