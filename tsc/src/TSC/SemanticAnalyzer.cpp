#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Base/Utilities.hpp"
#include "Case.hpp"
#include "SemanticAnalyzer.hpp"

namespace ProjectA {
	#define TRY(callback) { \
		const DiagnosticCode code = callback; \
		\
		if (code != DiagnosticCode::SUCCESS) { \
			BREAKPOINT(); \
			\
			return code; \
		} \
	}

	SemanticAnalyzer::SemanticAnalyzer(CompilationOutput& output) {
		this->output = &output;
		statementByteOffset = 0;
		expressionReferenceIndex = 0;
		expressionByteOffset = 0;
		memberDefinitionOffset = ~0;
		currentType = {Type::NONE, Type::NONE};
		returnType = {Type::NONE, Type::NONE};

		this->output->openScope({0, 0});

		while (statementByteOffset < this->output->ast.getStatementBufferByteSize()) {
			analyzeStatement();
		}

		this->output->closeScope(this->output->tokens.back().getRange().end);

		// Finished analyzing
		BREAKPOINT();
	}

	DiagnosticCode SemanticAnalyzer::analyzeStatement() {
		const uint32 byteOffset = statementByteOffset;
		const StatementType type = output->ast.getStatementType(byteOffset);

		switch (type) {
			case StatementType::STRUCTURE: {
				statementByteOffset += sizeof(Statement<StatementType::STRUCTURE>);

				return analyzeStructureStatement(output->ast.getStatement<StatementType::STRUCTURE>(byteOffset));
			}
			case StatementType::MEMBER: {
				statementByteOffset += sizeof(Statement<StatementType::MEMBER>);

				return analyzeMemberStatement(output->ast.getStatement<StatementType::MEMBER>(byteOffset));
			}
			case StatementType::CONSTANT: {
				statementByteOffset += sizeof(Statement<StatementType::CONSTANT>);

				return analyzeConstantStatement(output->ast.getStatement<StatementType::CONSTANT>(byteOffset));
			}
			case StatementType::VARIABLE: {
				statementByteOffset += sizeof(Statement<StatementType::VARIABLE>);

				return analyzeVariableStatement(output->ast.getStatement<StatementType::VARIABLE>(byteOffset));
			}
			case StatementType::PARAMETER: {
				statementByteOffset += sizeof(Statement<StatementType::PARAMETER>);

				return analyzeParameterStatement(output->ast.getStatement<StatementType::PARAMETER>(byteOffset));
			}
			case StatementType::FUNCTION: {
				statementByteOffset += sizeof(Statement<StatementType::FUNCTION>);

				return analyzeFunctionStatement(output->ast.getStatement<StatementType::FUNCTION>(byteOffset));
			}
			case StatementType::EXPRESSION: {
				statementByteOffset += sizeof(Statement<StatementType::EXPRESSION>);

				return analyzeExpressionStatement(output->ast.getStatement<StatementType::EXPRESSION>(byteOffset));
			}
			case StatementType::SCOPE: {
				statementByteOffset += sizeof(Statement<StatementType::SCOPE>);

				return analyzeScopeStatement(output->ast.getStatement<StatementType::SCOPE>(byteOffset));
			}
			case StatementType::IF: {
				statementByteOffset += sizeof(Statement<StatementType::IF>);

				return analyzeIfStatement(output->ast.getStatement<StatementType::IF>(byteOffset));
			}
			case StatementType::WHILE: {
				statementByteOffset += sizeof(Statement<StatementType::WHILE>);

				return analyzeWhileStatement(output->ast.getStatement<StatementType::WHILE>(byteOffset));
			}
			case StatementType::BREAK: {
				statementByteOffset += sizeof(Statement<StatementType::BREAK>);

				return analyzeBreakStatement(output->ast.getStatement<StatementType::BREAK>(byteOffset));
			}
			case StatementType::NEXT: {
				statementByteOffset += sizeof(Statement<StatementType::NEXT>);

				return analyzeNextStatement(output->ast.getStatement<StatementType::NEXT>(byteOffset));
			}
			case StatementType::RETURN: {
				statementByteOffset += sizeof(Statement<StatementType::RETURN>);

				return analyzeReturnStatement(output->ast.getStatement<StatementType::RETURN>(byteOffset));
			}
		}

		// TODO: Should never be here.
		BREAKPOINT();

		return DiagnosticCode::UNHANDLED;
	}

	DiagnosticCode SemanticAnalyzer::analyzeStructureStatement(const Statement<StatementType::STRUCTURE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		if (!isPascalCase(output->getIdentifier(identifierToken))) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		output->registerSemanticToken(SemanticTokenType::STRUCTURE, identifierToken.getRange());

		const uint32 definitionIndex = output->getDefinitionIndex(statement.identifierTokenIndex);

		if (definitionIndex != ~0) {
			const Definition& definition = output->getOpenScopeDefinition(definitionIndex);
			const Token& definitionToken = output->tokens[definition.identifierTokenIndex];
			const std::string definitionTokenString = output->getTokenString(definitionToken);

			// NOTE: Don't analyze redefined members.
			statementIndex += statement.memberCount;
			statementByteOffset += sizeof(Statement<StatementType::MEMBER>);
			expressionReferenceIndex += statement.memberCount;
			expressionByteOffset += sizeof(Expression<ExpressionType::TYPE>);

			return output->generateDiagnostic<DiagnosticCode::REDEFINITION>(identifierToken, output->getTokenString(identifierToken), definitionToken, definitionTokenString);
		}

		output->defineStructure(statement);

		if (statement.memberCount == 0) {
			return output->generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE>(identifierToken.getRange());
		}

		UserDefinedType& userDefinedType = output->getLastUserDefinedType();
		uint32 memberIndex = 0;
		uint32 memberDefinitionIndex = 0;
		uint32 lastMemberOffset = 0;
		uint32 lastMemberSize = 0;

		memberDefinitionOffset = output->getOpenScopeDefinitionCount();

		while (memberIndex < statement.memberCount) {
			memberIndex += 1;

			if (analyzeStatement() != DiagnosticCode::SUCCESS) {
				continue;
			}

			const Definition& definition = output->getOpenScopeDefinition(memberDefinitionOffset + memberDefinitionIndex);

			lastMemberOffset += lastMemberSize;
			lastMemberSize = output->getSize(definition.type);
			userDefinedType.alignment = max(userDefinedType.alignment, output->getAlignment(definition.type));

			memberDefinitionIndex += 1;
		}

		userDefinedType.size = roundUp(userDefinedType.alignment, lastMemberOffset + lastMemberSize);

		// TODO: Check that the "position" annotation is not used on multiple members.

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeMemberStatement(const Statement<StatementType::MEMBER>& statement) {
		analyzeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(expressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		// Type
		TRY(analyzeExpression());

		if (memberDefinitionOffset == 0 || memberDefinitionOffset == ~0) {
			return DiagnosticCode::UNHANDLED;
		}

		const Token& typeToken = output->tokens[typeExpression.typeTokenIndex];

		if (typeToken.type == TokenType::IDENTIFIER) {
			const Definition& definition = output->getOpenScopeDefinition(memberDefinitionOffset - 1);

			if (typeToken.index == definition.identifierIndex) {
				return output->generateDiagnostic<DiagnosticCode::INCOMPLETE_TYPE>(typeToken, output->getTokenString(typeToken));
			}
		}

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken))) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_MEMBER_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		output->registerSemanticToken(SemanticTokenType::MEMBER, statement.identifierTokenIndex);

		uint32 memberDefinitionIndex = memberDefinitionOffset;

		while (memberDefinitionIndex < output->getOpenScopeDefinitionCount()) {
			const Definition& definition = output->getOpenScopeDefinition(memberDefinitionIndex);

			if (identifierToken.index == definition.identifierIndex) {
				const Token& definitionToken = output->tokens[definition.identifierTokenIndex];
				const std::string definitionTokenString = output->getTokenString(definitionToken);
				const std::string redefinitionTokenString = output->getTokenString(identifierToken);

				return output->generateDiagnostic<DiagnosticCode::REDEFINITION>(identifierToken, redefinitionTokenString, definitionToken, definitionTokenString);
			}

			memberDefinitionIndex += 1;
		}

		output->defineMember(statement, currentType);

		if (statement.annotationFlag & BIT(POSITION) && currentType != PackedType(Type::VEC4, Type::FLOAT32)) {
			const AnnotationBlock& annotationBlock = output->ast.getAnnotationBlock<POSITION>(statement.annotationBlockIndexOffset, statement.annotationBlockCount);
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			return output->generateDiagnostic<DiagnosticCode::INVALID_ANNOTATION_TYPE_COMBINATION>(Annotation::POSITION, identifierToken, output->getPackedTypeString(currentType));
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeConstantStatement(const Statement<StatementType::CONSTANT>& statement) {
		analyzeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		// TODO: Constants can only have the "constant" annotation.

		// Type
		TRY(analyzeExpression());

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		if (!isScreamingSnakeCase(output->getIdentifier(identifierToken))) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		output->registerSemanticToken(SemanticTokenType::CONSTANT, statement.identifierTokenIndex);

		evaluation = ExpressionEvaluation(currentType, true);

		// Initializer
		TRY(analyzeExpression());

		if (!evaluation.constant) {
			return output->generateDiagnostic<DiagnosticCode::EXPRESSION_NOT_CONSTANT>(statement.initializerExpressionRange);
		}

		if (evaluation.intermediateType != evaluation.type) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(identifierToken);
		}

		Definition& definition = output->defineConstant(statement, currentType);
		definition.evaluatedInitializer = evaluation.getValue();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeVariableStatement(const Statement<StatementType::VARIABLE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];
		const std::string identifier = output->getIdentifier(identifierToken);

		if (!isCamelCaseOrSnakeCase(identifier) && !isScreamingSnakeCase(identifier)) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_VARIABLE_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		SemanticTokenType semanticTokenType = SemanticTokenType::VARIABLE;

		if (statement.constant) {
			semanticTokenType = SemanticTokenType::CONSTANT;
		}

		output->registerSemanticToken(semanticTokenType, statement.identifierTokenIndex);

		const uint32 definitionIndex = output->getDefinitionIndex(statement.identifierTokenIndex);

		if (definitionIndex != ~0) {
			const Definition& definition = output->getOpenScopeDefinition(definitionIndex);
			const Token& definitionToken = output->tokens[definition.identifierTokenIndex];
			const std::string definitionTokenString = output->getTokenString(definitionToken);

			return output->generateDiagnostic<DiagnosticCode::REDEFINITION>(identifierToken, output->getTokenString(identifierToken), definitionToken, definitionTokenString);
		}

		// Type
		TRY(analyzeExpression());

		output->defineVariableOrConstant(statement, currentType);

		if (statement.initializer) {
			evaluation = ExpressionEvaluation(currentType, statement.constant);

			// Initializer
			TRY(analyzeExpression());

			if (statement.constant && !evaluation.constant) {
				return output->generateDiagnostic<DiagnosticCode::EXPRESSION_NOT_CONSTANT>(statement.initializerExpressionRange);
			}

			if (evaluation.intermediateType != evaluation.type) {
				return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(identifierToken);
			}
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeParameterStatement(const Statement<StatementType::PARAMETER>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken))) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_PARAMETER_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		output->registerSemanticToken(SemanticTokenType::PARAMETER, statement.identifierTokenIndex);

		// Type
		TRY(analyzeExpression());

		// TODO: Search for redefinition errors.
		output->defineParameter(statement, currentType);
		output->markPreviousDefinitionForNextScope();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeFunctionStatement(const Statement<StatementType::FUNCTION>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];

		if (!isCamelCaseOrSnakeCase(output->getIdentifier(identifierToken))) {
			return output->generateDiagnostic<DiagnosticCode::INVALID_FUNCTION_IDENTIFIER_CASE>(identifierToken.getRange());
		}

		if (statement.returnType) {
			// Return type
			// TODO: Function return types cannot be annotated.
			TRY(analyzeExpression());

			returnType = currentType;
		}

		output->registerSemanticToken(SemanticTokenType::FUNCTION, statement.identifierTokenIndex);
		output->defineFunction(statement, expressionByteOffset);
		output->prepareNextScope();

		uint32 parameterIndex = 0;

		while (parameterIndex < statement.parameterCount) {
			// Parameter
			analyzeStatement();

			parameterIndex += 1;
		}

		// Scope
		// TODO: Search for return statement errors.
		analyzeStatement();

		// Cler return type.
		returnType = {Type::NONE, Type::NONE};

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement) {
		// NOTE: User-defined functions cannot be constant.
		evaluation = ExpressionEvaluation(returnType, false);

		// Condition
		TRY(analyzeExpression());

		if (evaluation.intermediateType != evaluation.type) {
			// TODO: Use correct token.
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(output->tokens[0]);
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeScopeStatement(const Statement<StatementType::SCOPE>& statement) {
		output->openScope(statement.range.start);

		uint32 statementIndex = 0;

		while (statementIndex < statement.statementCount) {
			analyzeStatement();

			statementIndex += 1;
		}

		output->closeScope(statement.range.end);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeIfStatement(const Statement<StatementType::IF>& statement) {
		// NOTE: User-defined functions cannot be constant.
		evaluation = ExpressionEvaluation(returnType, false);

		// Condition
		TRY(analyzeExpression());

		if (evaluation.intermediateType != evaluation.type) {
			// TODO: Use correct token.
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(output->tokens[0]);
		}

		// Scope
		TRY(analyzeStatement());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeWhileStatement(const Statement<StatementType::WHILE>& statement) {
		// NOTE: User-defined functions cannot be constant.
		evaluation = ExpressionEvaluation(returnType, false);

		// Condition
		TRY(analyzeExpression());

		if (evaluation.intermediateType != evaluation.type) {
			// TODO: Use correct token.
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(output->tokens[0]);
		}

		// Scope
		TRY(analyzeStatement());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeBreakStatement(const Statement<StatementType::BREAK>& statement) {
		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeNextStatement(const Statement<StatementType::NEXT>& statement) {
		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeReturnStatement(const Statement<StatementType::RETURN>& statement) {
		// NOTE: User-defined functions cannot be constant.
		evaluation = ExpressionEvaluation(returnType, false);

		TRY(analyzeExpression());

		if (evaluation.intermediateType != evaluation.type) {
			// TODO: Use correct token.
			return output->generateDiagnostic<DiagnosticCode::INVALID_CONVERSION>(output->tokens[0]);
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeExpression() {
		const ExpressionReference& expressionReference = output->ast.getExpressionReference(expressionReferenceIndex);

		expressionReferenceIndex += 1;
		expressionByteOffset = expressionReference.byteOffset;

		switch (expressionReference.type) {
			case ExpressionType::LITERAL: return analyzeLiteralExpression();
			case ExpressionType::IDENTIFIER: return analyzeIdentifierExpression();
			case ExpressionType::UNARY: return analyzeUnaryExpression();
			case ExpressionType::BINARY: return analyzeBinaryExpression();
			case ExpressionType::GROUP: return analyzeGroupExpression();
			case ExpressionType::MEMBER_ACCESS: return analyzeMemberAccessExpression();
			case ExpressionType::CALL: return analyzeCallExpression();
			case ExpressionType::TYPE: return analyzeTypeExpression();
		}

		// TODO: Should never be here.
		BREAKPOINT();

		return DiagnosticCode::UNHANDLED;
	}

	DiagnosticCode SemanticAnalyzer::analyzeLiteralExpression() {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(expressionByteOffset);
		const Token& literalToken = output->tokens[expression.tokenIndex];

		output->registerSemanticToken(SemanticTokenType::NUMBER, expression.tokenIndex);

		if (literalToken.type == TokenType::LITERAL_FLOATING_POINT) {
			if (evaluation.intermediateType.typeIndex != Type::FLOAT32 && evaluation.intermediateType.typeIndex != Type::FLOAT64) {
				return output->generateDiagnostic<DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT>(literalToken, output->getPackedTypeString(evaluation.intermediateType));
			}
		}

		evaluation.setValue(output->getLiteral(literalToken.index));

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeIdentifierExpression() {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(expressionByteOffset);
		const Token& identifierToken = output->tokens[expression.tokenIndex];
		const uint32 definitionIndex = output->getDefinitionIndex(expression.tokenIndex);

		if (definitionIndex == ~0) {
			return output->generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(identifierToken, output->getTokenString(identifierToken));
		}

		const Definition& definition = output->getOpenScopeDefinition(definitionIndex);

		output->registerSemanticToken(definition.semanticTokenType, expression.tokenIndex);
		output->createReference(definitionIndex, identifierToken);

		evaluation.setValue(definition.evaluatedInitializer);

		if (evaluation.constant) {
			evaluation.constant = definition.semanticTokenType == SemanticTokenType::CONSTANT;
		}

		// TODO: Generate appropriate error diagnostic if the evaluation isn't constant.

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeUnaryExpression() {
		const Expression<ExpressionType::UNARY>& expression = output->ast.getExpression<ExpressionType::UNARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		TRY(analyzeExpression());

		evaluation.setValue(applyUnaryOperator(evaluation.getValue(), static_cast<OperatorType>(operatorToken.type - TokenType::PLUS + UNARY_OPERATOR_OFFSET)));

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeBinaryExpression() {
		const Expression<ExpressionType::BINARY>& expression = output->ast.getExpression<ExpressionType::BINARY>(expressionByteOffset);
		const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

		evaluation = ExpressionEvaluation({Type::NONE, Type::NONE}, true);

		TRY(analyzeExpression());

		const ExpressionEvaluation lvalue = evaluation;

		evaluation = ExpressionEvaluation(lvalue.type, lvalue.constant);

		TRY(analyzeExpression());

		const ExpressionEvaluation& rvalue = evaluation;

		if (operatorToken.type >= TokenType::EQUAL && operatorToken.type <= TokenType::VERTICAL_BAR_EQUAL) {
			if (lvalue.constant && !rvalue.constant) {
				const Token& operatorToken = output->tokens[expression.operatorTokenIndex];

				return output->generateDiagnostic<DiagnosticCode::CONSTANT_ASSIGNMENT>(operatorToken.getRange());
			}

			return DiagnosticCode::SUCCESS;
		}

		evaluation.setValue(applyBinaryOperator(lvalue.getValue(), evaluation.getValue(), static_cast<OperatorType>(operatorToken.type - TokenType::VERTICAL_BAR + BINARY_OPERATOR_OFFSET)));

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeGroupExpression() {
		TRY(analyzeExpression());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeMemberAccessExpression() const {
		const Expression<ExpressionType::MEMBER_ACCESS>& expression = output->ast.getExpression<ExpressionType::MEMBER_ACCESS>(expressionByteOffset);

		// TODO
		BREAKPOINT();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeCallExpression() {
		const Expression<ExpressionType::CALL>& expression = output->ast.getExpression<ExpressionType::CALL>(expressionByteOffset);

		if (expression.explicitCallee) {
			// Identifier/Type
			TRY(analyzeExpression());
		}

		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			// Arguments
			TRY(analyzeExpression());

			argumentIndex += 1;
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeTypeExpression() {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionByteOffset);
		const Token& typeToken = output->tokens[expression.typeTokenIndex];

		Type templateArgument = Type::NONE;

		if (typeToken.type == TokenType::IDENTIFIER) {
			const uint32 typeIndex = output->getUserDefinedTypeIndex(typeToken.index);

			if (typeIndex == ~0) {
				return output->generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(typeToken, output->getTokenString(typeToken));
			}

			output->registerSemanticToken(SemanticTokenType::STRUCTURE, expression.typeTokenIndex);

			// TODO: Allow user-defined types to have template arguments in the context of uniforms/storage buffers.
			if (expression.templateArgumentTokenIndex != ~0) {
				const std::string typeTokenString = output->getTokenString(typeToken);
				const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];
				const std::string templateArgumentTokenString = output->getTokenString(templateArgumentToken);

				return output->generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(typeTokenString, templateArgumentToken, templateArgumentTokenString);
			}

			currentType = PackedType::userDefined(typeIndex, templateArgument);

			return DiagnosticCode::SUCCESS;
		}

		if (typeToken.type < FIRST_TYPE_INDEX || typeToken.type > LAST_TYPE_INDEX) {
			return output->generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(typeToken, output->getTokenString(typeToken));
		}

		const Type type = static_cast<Type>(typeToken.type - FIRST_TYPE_INDEX + 1);

		output->registerSemanticToken(SemanticTokenType::KEYWORD, expression.typeTokenIndex);

		if (expression.templateArgumentTokenIndex != ~0) {
			const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

			if (templateArgumentToken.type < FIRST_TYPE_INDEX || templateArgumentToken.type > LAST_TYPE_INDEX) {
				return output->generateDiagnostic<DiagnosticCode::UNDEFINED_TYPE>(templateArgumentToken, output->getTokenString(templateArgumentToken));
			}

			templateArgument = static_cast<Type>(templateArgumentToken.type - FIRST_TYPE_INDEX + 1);
		}

		const uint16 templateArgumentMask = TYPE_TEMPLATE_ARGUMENT_MASKS[static_cast<uint32>(type)];

		if (templateArgument > Type::NONE) {
			if (!(templateArgumentMask & BIT(templateArgument))) {
				const Token& templateArgumentToken = output->tokens[expression.templateArgumentTokenIndex];

				return output->generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(output->getTokenString(typeToken), templateArgumentToken, output->getTokenString(templateArgumentToken));
			}

			output->registerSemanticToken(SemanticTokenType::KEYWORD, expression.templateArgumentTokenIndex);

			currentType = PackedType(type, templateArgument);

			return DiagnosticCode::SUCCESS;
		}

		if (templateArgumentMask > 0) {
			return output->generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(typeToken, output->getTokenString(typeToken));
		}

		currentType = PackedType(type, templateArgument);

		return DiagnosticCode::SUCCESS;
	}

	void SemanticAnalyzer::analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) const {
		uint32 annotationBlockIndex = 0;
		uint32 annotationFlag = 0;

		while (annotationBlockIndex < annotationBlockCount) {
			const AnnotationBlock& annotationBlock = output->ast.getAnnotationBlock(annotationBlockIndexOffset + annotationBlockIndex);

			annotationBlockIndex += 1;

			if (analyzeAnnotationBlock(annotationBlock, annotationFlag) != DiagnosticCode::SUCCESS) {
				continue;
			}

			annotationFlag |= BIT(annotationBlock.annotation);

			output->registerSemanticToken(SemanticTokenType::PREDEFINED, annotationBlock.identifierTokenIndex);
		}
	}

	DiagnosticCode SemanticAnalyzer::analyzeAnnotationBlock(const AnnotationBlock& annotationBlock, uint32 annotationFlag) const {
		if (annotationFlag & BIT(annotationBlock.annotation)) {
			const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

			return output->generateDiagnostic<DiagnosticCode::DUPLICATE_ANNOTATION>(identifierToken, output->getTokenString(identifierToken));
		}

		switch (annotationBlock.annotation) {
			case Annotation::BINDING:
			case Annotation::CONSTANT:
			case Annotation::LOCATION:
			case Annotation::SET: {
				if (annotationBlock.valueTokenIndex == ~0) {
					const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

					return output->generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(identifierToken, output->getTokenString(identifierToken));
				}

				output->registerSemanticToken(SemanticTokenType::NUMBER, annotationBlock.valueTokenIndex);

				break;
			}
			default: {
				break;
			}
		}

		return DiagnosticCode::SUCCESS;
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
}