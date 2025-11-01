#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Diagnostic/Warning.hpp"
#include "Language/Type.hpp"
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
		currentExpressionReferenceIndex = 0;
	}

	void SemanticAnalyzer::analyzeStructureStatement(uint32 statementIndex, const ref<StructureStatement>& statement, DocumentPosition end, uint32 memberTypeExpressionReferenceOffset) {
		UserDefinedType& userDefinedType = output->defineStructure(statement, end);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];

		if (!isPascalCase(output->getIdentifier(identifierToken))) {
			generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(*output, identifierToken);
		}
		else {
			output->registerSemanticToken(SemanticTokenType::STRUCTURE, statement->identifierTokenIndex);
		}

		if (statement->memberCount == 0) {
			generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE>(*output, identifierToken);
		}

		const uint32 memberStatementOffset = statementIndex + 1;
		uint32 lastMemberOffset = 0;
		uint32 lastMemberSize = 0;

		for (uint32 memberIndex = 0; memberIndex < statement->memberCount; memberIndex += 1) {
			const ref<MemberStatement>& memberStatement = std::static_pointer_cast<MemberStatement>(output->ast.statements[memberStatementOffset + memberIndex]);
			const ExpressionReference& expressionReference = output->ast.getExpressionReference(memberTypeExpressionReferenceOffset + memberIndex);
			const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionReference.byteOffset);

			output->defineMember(memberStatement, expression);

			lastMemberOffset += lastMemberSize;
			lastMemberSize = output->getSize(expression.type);

			#ifdef __EMSCRIPTEN__
				userDefinedType.alignment = userDefinedType.alignment > output->getAlignment(expression.type) ? userDefinedType.alignment : output->getAlignment(expression.type);
			#else
				userDefinedType.alignment = max(userDefinedType.alignment, output->getAlignment(expression.type));
			#endif
		}

		userDefinedType.size = roundUp(userDefinedType.alignment, lastMemberOffset + lastMemberSize);

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

	void SemanticAnalyzer::analyzeMemberStatement(uint32 structureStatementIndex, const ref<StructureStatement>& structureStatement, const ref<MemberStatement>& statement) {
		analyzeAnnotationBlocks(statement->annotationBlockIndexOffset, statement->annotationBlockCount);

		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		if (analyzeTypeExpression(typeExpression) != DiagnosticCode::SUCCESS) {
			return;
		}

		output->registerSemanticToken(SemanticTokenType::MEMBER, statement->identifierTokenIndex);

		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string identifierTokenString = output->getTokenString(identifierToken);
		const uint32 memberStatementOffset = structureStatementIndex + 1;

		for (uint32 memberIndex = 0; memberIndex < structureStatement->memberCount; memberIndex += 1) {
			const ref<MemberStatement> memberStatement = std::static_pointer_cast<MemberStatement>(output->ast.statements[memberStatementOffset + memberIndex]);
			const Token& definitionToken = output->tokens[memberStatement->identifierTokenIndex];

			if (identifierTokenString == output->getTokenString(definitionToken)) {
				generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, identifierToken, memberStatement->identifierTokenIndex);

				break;
			}
		}

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken))) {
			generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		if (statement->annotationFlag & BIT(static_cast<uint32>(Annotation::POSITION))) {
			const PackedType type = typeExpression.type;

			if (type.typeIndex != Type::VEC4 || type.templateArgumentIndex != Type::FLOAT32) {
				const AnnotationBlock& annotationBlock = getAnnotationBlock(statement->annotationBlockIndexOffset, statement->annotationBlockIndexOffset, Annotation::POSITION);

				generateDiagnostic<DiagnosticCode::INVALID_ANNOTATION_TYPE_COMBINATION>(*output, Annotation::POSITION, type, annotationBlock.identifierTokenIndex);
			}
		}
	}

	void SemanticAnalyzer::analyzeConstantStatement(const ref<ConstantStatement>& statement) {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		if (analyzeTypeExpression(typeExpression) != DiagnosticCode::SUCCESS) {
			return;
		}

		if (!isScreamingSnakeCase(output->getIdentifier(identifierToken))) {
			generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE>(*output, identifierToken);

			return;
		}
		else {
			output->registerSemanticToken(SemanticTokenType::CONSTANT, statement->identifierTokenIndex);
		}

		Definition& definition = output->defineConstant(statement, typeExpression);

		ExpressionEvaluation evaluation{};
		evaluation.type = typeExpression.type;
		evaluation.intermediateType = typeExpression.type;
		evaluation.casting = false;
		evaluation.constant = true;

		// TODO
		/*
			Start the evaluation with type = constant type.

			If different type found:
			- If converting is true, OK
			- Else, conversion error

			If constructor found:
			- compare the number of arguments with each overload of the constructor/function
			- info diagnostic if same type
			- set type to constructor type
			- set converting to true
		*/

		evaluateNextExpression(evaluation);

		if (!evaluation.constant) {
			// TODO: Provide the expression range instead of the identifier range.
			generateDiagnostic<DiagnosticCode::EXPRESSION_NOT_CONSTANT>(*output, identifierToken.start, identifierToken.getEnd());

			return;
		}

		if (evaluation.intermediateType != typeExpression.type) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, identifierToken);

			return;
		}

		definition.evaluatedInitializer = evaluation.components.back();
	}

	void SemanticAnalyzer::analyzeFunctionStatement(const ref<FunctionStatement>& statement, uint32 parameterTypeExpressionOffset, DocumentPosition end) {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string& identifier = output->getIdentifier(identifierToken);

		if (!isCamelCaseOrSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE>(*output, identifierToken);

			return;
		}

		if (statement->returnType) {
			const ExpressionReference& returnTypeExpressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);
			const Expression<ExpressionType::TYPE>& returnTypeExpression = output->ast.getExpression<ExpressionType::TYPE>(returnTypeExpressionReference.byteOffset);

			if (analyzeTypeExpression(returnTypeExpression) != DiagnosticCode::SUCCESS) {
				return;
			}
		}

		output->registerSemanticToken(SemanticTokenType::FUNCTION, statement->identifierTokenIndex);
		output->defineFunction(statement, parameterTypeExpressionOffset, end);

		// TODO
		/* if (statementIndex == 0) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);

			return;
		} */

		// TODO: Check that there is no return statement.
		/* const uint32 previousStatementIndex = statementIndex - 1;
		const ref<Statement> previousStatement = output->ast.statements[previousStatementIndex];

		if (previousStatement->statementType != StatementType::RETURN) {
			generateDiagnostic<DiagnosticCode::NO_RETURN>(*output, identifierToken);
		} */
	}

	void SemanticAnalyzer::analyzeParameterStatement(const ref<ParameterStatement>& statement) const {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string& identifier = output->getIdentifier(identifierToken);

		output->registerSemanticToken(SemanticTokenType::PARAMETER, statement->identifierTokenIndex);

		if (!isCamelCaseOrSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		// TODO
		/* const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(statement->typeExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		output->define(typeExpression.type, SemanticTokenType::PARAMETER, 0, statement->identifierTokenIndex, identifierToken.getEnd()); */
		output->nextScopeDefinitionCount += 1;
	}

	void SemanticAnalyzer::analyzeVariableStatement(const ref<VariableStatement>& statement) {
		const Token& identifierToken = output->tokens[statement->identifierTokenIndex];
		const std::string& identifier = output->getIdentifier(identifierToken);

		if (!isCamelCaseOrSnakeCase(identifier) && !isScreamingSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE>(*output, identifierToken);

			// NOTE: This situation is invalid, but we still continue to help the user search for references to replace.
		}

		if (statement->constant) {
			output->registerSemanticToken(SemanticTokenType::CONSTANT, statement->identifierTokenIndex);
		}
		else {
			output->registerSemanticToken(SemanticTokenType::VARIABLE, statement->identifierTokenIndex);
		}

		const Definition* previousDefinition = output->getFirstDefinition(identifier);

		if (previousDefinition != nullptr) {
			generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, identifierToken, previousDefinition->tokenIndex);

			return;
		}

		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		if (analyzeTypeExpression(typeExpression) != DiagnosticCode::SUCCESS) {
			return;
		}

		output->defineVariableOrConstant(statement, typeExpression);

		ExpressionEvaluation evaluation{};
		evaluation.type = typeExpression.type;
		evaluation.intermediateType = typeExpression.type;
		evaluation.casting = false;
		evaluation.constant = statement->constant;

		evaluateNextExpression(evaluation);

		if (statement->constant && !evaluation.constant) {
			// TODO: Provide the expression range instead of the identifier range.
			generateDiagnostic<DiagnosticCode::EXPRESSION_NOT_CONSTANT>(*output, identifierToken.start, identifierToken.getEnd());

			return;
		}

		// TODO: Also check vector/matrix/user-defined types.
		if (evaluation.intermediateType != typeExpression.type) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, identifierToken);

			return;
		}
	}

	void SemanticAnalyzer::analyzeReturnStatement(const ref<ReturnStatement>& statement) {
		// TODO
		const PackedType type(Type::NONE, Type::NONE);

		ExpressionEvaluation evaluation{};
		evaluation.type = type;
		evaluation.intermediateType = type;
		evaluation.casting = false;
		evaluation.constant = true;

		evaluateNextExpression(evaluation);

		// TODO
		/* if (evaluation.intermediateType != type) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, identifierToken);

			return;
		} */
	}

	void SemanticAnalyzer::analyzeIdentifierExpression(const Expression<ExpressionType::IDENTIFIER>& expression) {
		currentExpressionReferenceIndex += 1;

		const Token& identifierToken = output->tokens[expression.tokenIndex];
		const Definition* definition = output->getFirstDefinition(output->getIdentifier(identifierToken));

		if (definition == nullptr) {
			if (identifierToken.type < FIRST_FUNCTION_INDEX || identifierToken.type > LAST_FUNCTION_INDEX) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, identifierToken);

				return;
			}

			output->registerSemanticToken(SemanticTokenType::PREDEFINED, expression.tokenIndex);

			return;
		}

		output->registerSemanticToken(definition->semanticTokenType, expression.tokenIndex);
	}

	void SemanticAnalyzer::analyzeMemberAccessExpression(const Expression<ExpressionType::MEMBER_ACCESS>& expression) {
		currentExpressionReferenceIndex += 1;

		const Token& identifierToken = output->tokens[expression.identifierTokenIndex];
		const Definition* definition = output->getFirstDefinition(output->getIdentifier(identifierToken));

		if (definition == nullptr) {
			generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, identifierToken);

			return;
		}
	}

	DiagnosticCode SemanticAnalyzer::analyzeTypeExpression(const Expression<ExpressionType::TYPE>& expression) {
		currentExpressionReferenceIndex += 1;

		if (expression.type.hasUserDefinedType()) {
			if (!output->isValidUserDefinedType(expression.type)) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(*output, expression.typeTokenIndex);

				return DiagnosticCode::UNDEFINED_TYPE;
			}

			if (expression.type.templateArgumentIndex > 0) {
				const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex, expression.templateArgumentTokenIndex);

				return DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;
			}

			output->registerSemanticToken(SemanticTokenType::STRUCTURE, expression.typeTokenIndex);

			return DiagnosticCode::SUCCESS;
		}

		const uint16 templateArgumentMask = TYPE_TEMPLATE_ARGUMENT_MASKS[expression.type.typeIndex];

		if (expression.type.templateArgumentIndex == 0) {
			if (templateArgumentMask == 0) {
				return DiagnosticCode::SUCCESS;
			}

			generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex);

			return DiagnosticCode::MISSING_TEMPLATE_ARGUMENT;
		}

		if (templateArgumentMask & BIT(expression.type.templateArgumentIndex)) {
			return DiagnosticCode::SUCCESS;
		}

		generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex, expression.templateArgumentTokenIndex);

		return DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;
	}

	const AnnotationBlock& SemanticAnalyzer::getAnnotationBlock(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount, Annotation annotation) const {
		DEBUG_ASSERT(annotationBlockCount > 0);

		uint32 annotationBlockIndex = 0;

		while (annotationBlockIndex < annotationBlockCount - 1) {
			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];

			if (annotationBlock.annotation == annotation) {
				return annotationBlock;
			}

			annotationBlockIndex += 1;
		}

		DEBUG_ASSERT(false);

		// should never be here
		return output->ast.annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];
	}

	void SemanticAnalyzer::analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const {
		uint32 uniqueAnnotationFlag = 0;

		for (uint32 annotationBlockIndex = 0; annotationBlockIndex < annotationBlockCount; annotationBlockIndex += 1) {
			const AnnotationBlock& annotationBlock = output->ast.annotationBlocks[annotationBlockIndexOffset + annotationBlockIndex];
			const uint32 annotationBit = BIT(annotationBlock.annotation);

			if (uniqueAnnotationFlag & annotationBit) {
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

			uniqueAnnotationFlag |= annotationBit;
		}
	}

	void SemanticAnalyzer::evaluateNextExpression(ExpressionEvaluation& evaluation) {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);

		currentExpressionReferenceIndex += 1;

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: return evaluateLiteralExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::IDENTIFIER: return evaluateIdentifierExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::UNARY: return evaluateUnaryExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::BINARY: return evaluateBinaryExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::GROUP: return evaluateGroupExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::MEMBER_ACCESS: return evaluateMemberAccessExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::CALL: return evaluateCallExpression(evaluation, expressionReference.byteOffset);
			case ExpressionType::TYPE: return evaluateTypeExpression(evaluation, expressionReference.byteOffset);
		}
	}

	void SemanticAnalyzer::evaluateLiteralExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(byteOffset);
		const Token& token = output->tokens[expression.tokenIndex];
		const sint64 component = output->getLiteral(token.index);

		evaluation.components.back() = component;

		if (token.type == TokenType::LITERAL_FLOATING_POINT && evaluation.intermediateType.typeIndex != Type::FLOAT32 && evaluation.intermediateType.typeIndex != Type::FLOAT64) {
			generateDiagnostic<DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT>(*output, token, evaluation.intermediateType);

			// TODO: Stop evaluating the expression.
			return;
		}
	}

	void SemanticAnalyzer::evaluateIdentifierExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		// TODO
		BREAKPOINT();
	}

	void SemanticAnalyzer::evaluateUnaryExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		evaluateNextExpression(evaluation);

		evaluation.components.back() += (applyUnaryOperator(evaluation.components.back(), static_cast<OperatorType>(operatorToken.type - TokenType::PLUS)));
	}

	void SemanticAnalyzer::evaluateBinaryExpression(ExpressionEvaluation& leftEvaluation, uint32 byteOffset) {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(byteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		ExpressionEvaluation rightEvaluation{};
		rightEvaluation.constant = true;
		rightEvaluation.components.back() = 0;

		evaluateNextExpression(leftEvaluation);
		evaluateNextExpression(rightEvaluation);

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
				leftEvaluation.constant = false;

				return;
			}
			default: {
				break;
			}
		}

		leftEvaluation.components.back() = applyBinaryOperator(leftEvaluation.components.back(), rightEvaluation.components.back(), static_cast<OperatorType>(operatorToken.type - TokenType::VERTICAL_BAR));
	}

	void SemanticAnalyzer::evaluateGroupExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		const Expression<ExpressionType::GROUP>& expression = output->ast.getExpression<ExpressionType::GROUP>(byteOffset);

		evaluateNextExpression(evaluation);
	}

	void SemanticAnalyzer::evaluateMemberAccessExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		// TODO
		BREAKPOINT();
	}

	void SemanticAnalyzer::evaluateCallExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(byteOffset);

		if (expression.explicitCallee) {
			const ExpressionReference& calleeExpressionReference = output->ast.getExpressionReference(currentExpressionReferenceIndex);

			currentExpressionReferenceIndex += 1;

			switch (calleeExpressionReference.type) {
				case ExpressionType::IDENTIFIER: {
					// Function call
					const Expression<ExpressionType::IDENTIFIER>& identifierExpression = output->ast.getExpression<ExpressionType::IDENTIFIER>(calleeExpressionReference.byteOffset);
					const Token& identifierToken = output->tokens[identifierExpression.tokenIndex];
					const uint32 functionIndex = output->getFunctionIndex(identifierToken.index);

					if (functionIndex == ~0) {
						generateDiagnostic<DiagnosticCode::UNDEFINED_FUNCTION>(*output, identifierToken);

						return;
					}

					// TODO
					// 3. For each overload, compare the parameter count and types with the actual arguments provided in the call expression.
					// 4. If no overload match (count isn't the same OR a type doesn't match), generate UNDEFINED_OVERLOAD diagnostic.
					const Function& function = output->functions[functionIndex];
					uint32 overloadIndex = 0;
					bool resolvedOverload = false;

					while (overloadIndex < function.overloads.size()) {
						const FunctionOverload& overload = function.overloads[overloadIndex];
						const uint32 parameterCount = static_cast<uint32>(overload.parameters.size());

						uint32 parameterIndex = 0;

						// NOTE
						// Even if the parameter count is different, we must compare the overload
						// because the component count of the unwrapped arguments might still match.
						while (parameterIndex < parameterCount) {
							const FunctionOverloadParameter& parameter = overload.parameters[parameterIndex];

							ExpressionEvaluation argumentEvaluation{};
							argumentEvaluation.type = parameter.type;
							argumentEvaluation.intermediateType = parameter.type;
							argumentEvaluation.constant = false;
							argumentEvaluation.casting = false;

							evaluateNextExpression(argumentEvaluation);

							parameterIndex += static_cast<uint32>(argumentEvaluation.components.size());
						}

						if (parameterCount == expression.argumentCount) {
							resolvedOverload = true;

							break;
						}

						overloadIndex += 1;
					}

					if (!resolvedOverload) {
						// TODO: Also compare unwrapped argument count.
						generateDiagnostic<DiagnosticCode::UNDEFINED_FUNCTION_OVERLOAD>(*output, identifierToken);

						return;
					}

					BREAKPOINT();

					break;
				}
				case ExpressionType::TYPE: {
					// Explicit constructor call
					const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(calleeExpressionReference.byteOffset);
					const PackedType type = typeExpression.type;

					const uint32 expectedArgumentCount = output->getComponentCount(type);

					if (expression.argumentCount != expectedArgumentCount) {
						generateDiagnostic<DiagnosticCode::ARGUMENT_COUNT_MISMATCH>(*output, type, expectedArgumentCount, expression.argumentCount);

						return;
					}

					BREAKPOINT();

					break;
				}
				default: {
					break;
				}
			}

			evaluateNextExpression(evaluation);
		}
		else {
			// Implicit constructor call
			const PackedType type = evaluation.type;
			const uint32 expectedArgumentCount = output->getComponentCount(type);

			if (expression.argumentCount != expectedArgumentCount) {
				generateDiagnostic<DiagnosticCode::ARGUMENT_COUNT_MISMATCH>(*output, type, expectedArgumentCount, expression.argumentCount);

				return;
			}
		}

		BREAKPOINT();

		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			ExpressionEvaluation argumentEvaluation{};

			evaluateNextExpression(argumentEvaluation);

			// TODO
			// evaluation.components.emplace_back(argumentEvaluation.components);

			argumentIndex += 1;
		}
	}

	void SemanticAnalyzer::evaluateTypeExpression(ExpressionEvaluation& evaluation, uint32 byteOffset) {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(byteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		if (!evaluation.casting && expression.type != evaluation.intermediateType) {
			generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(*output, typeToken);

			// TODO: Stop evaluation.
			return;
		}

		evaluation.intermediateType = expression.type;
		evaluation.casting = true;
	}

	sint64 SemanticAnalyzer::applyBinaryOperator(sint64 lvalue, sint64 rvalue, OperatorType operatorType) const {
		switch (operatorType) {
			case OperatorType::VERTICAL_BAR: {
				return lvalue |= rvalue;
			}
			case OperatorType::CARET: {
				return lvalue ^= rvalue;
			}
			case OperatorType::AMPERSAND: {
				return lvalue &= rvalue;
			}
			case OperatorType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: {
				return lvalue <<= rvalue;
			}
			case OperatorType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: {
				return lvalue >>= rvalue;
			}
			case OperatorType::EQUAL_EQUAL: {
				return lvalue == rvalue;
			}
			case OperatorType::EXCLAMATION_EQUAL: {
				return lvalue != rvalue;
			}
			case OperatorType::LEFT_ANGLE_BRACKET: {
				return lvalue < rvalue;
			}
			case OperatorType::LEFT_ANGLE_BRACKET_EQUAL: {
				return lvalue <= rvalue;
			}
			case OperatorType::RIGHT_ANGLE_BRACKET: {
				return lvalue > rvalue;
			}
			case OperatorType::RIGHT_ANGLE_BRACKET_EQUAL: {
				return lvalue >= rvalue;
			}
			case OperatorType::ASTERISK: {
				return lvalue *= rvalue;
			}
			case OperatorType::SLASH: {
				return lvalue /= rvalue;
			}
			case OperatorType::PLUS: {
				return lvalue += rvalue;
			}
			case OperatorType::MINUS: {
				return lvalue -= rvalue;
			}
			default: {
				DEBUG_ASSERT(false);

				return 0;
			}
		}
	}

	sint64 SemanticAnalyzer::applyUnaryOperator(sint64 rvalue, OperatorType operatorType) const {
		switch (operatorType) {
			case OperatorType::PLUS: {
				return rvalue;
			}
			case OperatorType::MINUS: {
				return -rvalue;
			}
			case OperatorType::EXCLAMATION: {
				return !rvalue;
			}
			case OperatorType::TILDE: {
				return ~rvalue;
			}
			default: {
				DEBUG_ASSERT(false);

				return 0;
			}
		}
	}
}