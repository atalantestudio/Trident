#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Base/Utilities.hpp"
#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Diagnostic/Warning.hpp"
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
		statementIndex = 0;
		statementByteOffset = 0;
		expressionReferenceIndex = 0;
		expressionByteOffset = 0;
		memberDefinitionOffset = ~0;
		position = {0, 0};

		this->output->openScope(position);

		while (statementIndex < this->output->ast.getStatementCount()) {
			analyzeStatement();
		}

		this->output->closeScope(position);

		// Finished analyzing
		BREAKPOINT();
	}

	DiagnosticCode SemanticAnalyzer::analyzeStatement() {
		const uint32 byteOffset = statementByteOffset;
		const StatementType type = output->ast.getStatementType(byteOffset);

		statementIndex += 1;

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

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeStructureStatement(const Statement<StatementType::STRUCTURE>& statement) {
		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];
		const std::string identifier = output->getIdentifier(identifierToken);

		if (!isPascalCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE>(*output, identifierToken);

			return DiagnosticCode::INVALID_STRUCTURE_IDENTIFIER_CASE;
		}

		UserDefinedType& userDefinedType = output->defineStructure(statement);
		uint32 memberIndex = 0;
		uint32 lastMemberOffset = 0;
		uint32 lastMemberSize = 0;

		memberDefinitionOffset = output->getOpenScopeDefinitionCount();

		while (memberIndex < statement.memberCount) {
			if (analyzeStatement() != DiagnosticCode::SUCCESS) {
				continue;
			}

			const Definition& definition = output->getOpenScopeDefinition(memberDefinitionOffset + memberIndex);

			lastMemberOffset += lastMemberSize;
			lastMemberSize = output->getSize(definition.type);
			userDefinedType.alignment = max(userDefinedType.alignment, output->getAlignment(definition.type));

			memberIndex += 1;
		}

		if (statement.memberCount == 0) {
			generateDiagnostic<DiagnosticCode::EMPTY_STRUCTURE>(*output, identifierToken);
		}

		userDefinedType.size = roundUp(userDefinedType.alignment, lastMemberOffset + lastMemberSize);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeMemberStatement(const Statement<StatementType::MEMBER>& statement) {
		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(expressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		// Type
		TRY(analyzeExpression());

		if (memberDefinitionOffset == ~0) {
			return DiagnosticCode::UNHANDLED;
		}

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];
		const std::string identifier = output->getIdentifier(identifierToken);

		uint32 memberDefinitionIndex = memberDefinitionOffset;

		while (memberDefinitionIndex < output->getOpenScopeDefinitionCount()) {
			const Definition& definition = output->getOpenScopeDefinition(memberDefinitionIndex);

			if (identifier == definition.identifier) {
				generateDiagnostic<DiagnosticCode::REDEFINITION>(*output, identifierToken, definition.tokenIndex);

				return DiagnosticCode::REDEFINITION;
			}

			memberDefinitionIndex += 1;
		}

		analyzeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		output->defineMember(statement, typeExpression);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeConstantStatement(const Statement<StatementType::CONSTANT>& statement) {
		const ExpressionReference& typeExpressionReference = output->ast.getExpressionReference(expressionReferenceIndex);
		const Expression<ExpressionType::TYPE>& typeExpression = output->ast.getExpression<ExpressionType::TYPE>(typeExpressionReference.byteOffset);

		analyzeAnnotationBlocks(statement.annotationBlockIndexOffset, statement.annotationBlockCount);

		// Type
		TRY(analyzeExpression());

		const Token& identifierToken = output->tokens[statement.identifierTokenIndex];
		const std::string identifier = output->getIdentifier(identifierToken);

		if (!isScreamingSnakeCase(identifier)) {
			generateDiagnostic<DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE>(*output, identifierToken);

			return DiagnosticCode::INVALID_CONSTANT_IDENTIFIER_CASE;
		}

		evaluation = ExpressionEvaluation(typeExpression.type, true);

		// Initializer
		TRY(analyzeExpression());

		output->registerSemanticToken(SemanticTokenType::CONSTANT, statement.identifierTokenIndex);

		Definition& definition = output->defineConstant(statement, typeExpressionReference);
		definition.evaluatedInitializer = evaluation.getValue();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeVariableStatement(const Statement<StatementType::VARIABLE>& statement) {
		// Type
		TRY(analyzeExpression());

		if (statement.initializer) {
			// Initializer
			BREAKPOINT();
			TRY(analyzeExpression());
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeParameterStatement(const Statement<StatementType::PARAMETER>& statement) {
		// TODO
		BREAKPOINT();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeFunctionStatement(const Statement<StatementType::FUNCTION>& statement) {
		if (statement.returnType) {
			// Return type
			// TODO: It should not be possible to annotate the return type of a function.
			TRY(analyzeExpression());
		}

		uint32 parameterIndex = 0;

		while (parameterIndex < statement.parameterCount) {
			// Parameter
			analyzeStatement();

			parameterIndex += 1;
		}

		// Scope
		analyzeStatement();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeExpressionStatement(const Statement<StatementType::EXPRESSION>& statement) {
		return analyzeExpression();
	}

	DiagnosticCode SemanticAnalyzer::analyzeScopeStatement(const Statement<StatementType::SCOPE>& statement) {
		output->openScope(statement.range.start);

		uint32 statementIndex = 0;

		while (statementIndex < statement.statementCount) {
			analyzeStatement();

			statementIndex += 1;
		}

		output->openScope(statement.range.end);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeIfStatement(const Statement<StatementType::IF>& statement) {
		// Condition
		TRY(analyzeExpression());

		// Scope
		return analyzeStatement();
	}

	DiagnosticCode SemanticAnalyzer::analyzeWhileStatement(const Statement<StatementType::WHILE>& statement) {
		// Condition
		TRY(analyzeExpression());

		// Scope
		return analyzeStatement();
	}

	DiagnosticCode SemanticAnalyzer::analyzeBreakStatement(const Statement<StatementType::BREAK>& statement) {
		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeNextStatement(const Statement<StatementType::NEXT>& statement) {
		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeReturnStatement(const Statement<StatementType::RETURN>& statement) {
		return analyzeExpression();
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

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeLiteralExpression() {
		const Expression<ExpressionType::LITERAL>& expression = output->ast.getExpression<ExpressionType::LITERAL>(expressionByteOffset);
		const Token& literalToken = output->tokens[expression.tokenIndex];

		output->registerSemanticToken(SemanticTokenType::NUMBER, expression.tokenIndex);

		if (literalToken.type == TokenType::LITERAL_FLOATING_POINT) {
			if (evaluation.intermediateType.typeIndex != Type::FLOAT32 && evaluation.intermediateType.typeIndex != Type::FLOAT64) {
				generateDiagnostic<DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT>(*output, literalToken, evaluation.intermediateType);

				return DiagnosticCode::INVALID_FLOATING_POINT_ASSIGNMENT;
			}
		}

		evaluation.setValue(output->getLiteral(literalToken.index));

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeIdentifierExpression() {
		const Expression<ExpressionType::IDENTIFIER>& expression = output->ast.getExpression<ExpressionType::IDENTIFIER>(expressionByteOffset);
		const uint32 definitionIndex = output->getDefinitionIndex(expression.tokenIndex);

		if (definitionIndex == ~0) {
			generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, expression.tokenIndex);

			return DiagnosticCode::UNDEFINED_IDENTIFIER;
		}

		output->createReference(definitionIndex, expression.tokenIndex);

		const Definition& definition = output->getOpenScopeDefinition(definitionIndex);

		evaluation.setValue(definition.evaluatedInitializer);

		// TODO: If the definition isn't constant, generate error diagnostic.

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

		TRY(analyzeExpression());

		const sint64 lvalue = evaluation.getValue();

		TRY(analyzeExpression());

		evaluation.setValue(applyBinaryOperator(lvalue, evaluation.getValue(), static_cast<OperatorType>(operatorToken.type - TokenType::VERTICAL_BAR + BINARY_OPERATOR_OFFSET)));

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
			// Explicit
			// Identifier/Type
			TRY(analyzeExpression());
		}
		else {
			// Implicit
		}

		uint32 argumentIndex = 0;

		while (argumentIndex < expression.argumentCount) {
			// Arguments
			TRY(analyzeExpression());

			argumentIndex += 1;
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode SemanticAnalyzer::analyzeTypeExpression() const {
		const Expression<ExpressionType::TYPE>& expression = output->ast.getExpression<ExpressionType::TYPE>(expressionByteOffset);

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

			// output->registerSemanticToken(SemanticTokenType::STRUCTURE, expression.typeTokenIndex);

			return DiagnosticCode::SUCCESS;
		}

		const uint16 templateArgumentMask = TYPE_TEMPLATE_ARGUMENT_MASKS[expression.type.typeIndex];

		if (!(templateArgumentMask & BIT(expression.type.templateArgumentIndex))) {
			if (expression.type.templateArgumentIndex > Type::NONE) {
				generateDiagnostic<DiagnosticCode::INVALID_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex, expression.templateArgumentTokenIndex);

				return DiagnosticCode::INVALID_TEMPLATE_ARGUMENT;
			}

			if (templateArgumentMask > 0) {
				generateDiagnostic<DiagnosticCode::MISSING_TEMPLATE_ARGUMENT>(*output, expression.typeTokenIndex);

				return DiagnosticCode::MISSING_TEMPLATE_ARGUMENT;
			}
		}

		output->registerSemanticToken(SemanticTokenType::KEYWORD, expression.typeTokenIndex);
		// TODO: Register another semantic token for the template argument.

		return DiagnosticCode::SUCCESS;
	}

	void SemanticAnalyzer::analyzeAnnotationBlocks(uint32 annotationBlockIndexOffset, uint32 annotationBlockCount) {
		uint32 annotationBlockIndex = 0;

		while (annotationBlockIndex < annotationBlockCount) {
			// TODO
			BREAKPOINT();

			annotationBlockIndex += 1;
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