#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Parser.hpp"

namespace ProjectA {
	#define EXPECT(condition, message) \
		if (!(condition)) { \
			output->generateDiagnostic(tokenIndex, message, DiagnosticSeverity::ERROR); \
			\
			return false; \
		}

	#define EXPECT2(condition, diagnosticCode, ...) \
		if (!(condition)) { \
			generateDiagnostic<diagnosticCode>(*output, __VA_ARGS__); \
			\
			BREAKPOINT(); \
			\
			return false; \
		}

	#define BINARY_EXPRESSION(condition, callback) \
		if (!callback()) { \
			return false; \
		} \
		\
		while (condition) { \
			Expression<ExpressionType::BINARY> expression{}; \
			expression.leftExpressionReferenceIndex = getLastExpressionReferenceIndex(); \
			expression.operatorTokenIndex = tokenIndex; \
			\
			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0); \
			\
			jump(); \
			\
			if (!callback()) { \
				return false; \
			} \
			\
			expression.rightExpressionReferenceIndex = getLastExpressionReferenceIndex(); \
			\
			output->ast.addExpression(expression); \
		} \
		\
		return true;

	void Parser::parse(CompilationOutput* _output) {
		output = _output;
		output->ast = {};
		tokenIndex = 0;
		currentAnnotationCount = 0;
		annotationFlag = 0;
		position = {};
		parentStatementIndex = ~0;
		visitedWhileCount = 0;

		analyzer.setCompilationOutput(output);
		output->openScope(position);

		jumpEol();

		while (read().type != TokenType::END_OF_FILE) {
			if (!parseGlobalScopeStatement()) {
				synchronize();
			}

			jumpEol();
		}

		output->closeScope(position);
	}

	bool Parser::parseGlobalScopeStatement() {
		if (read().type >= TokenType::USER_DEFINED && peek().type == TokenType::LEFT_CURLY_BRACE) {
			return parseStructureStatement();
		}

		if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			if (!parseAnnotationBlocks()) {
				return false;
			}
		}

		return parseConstantOrFunctionStatement();
	}

	bool Parser::parseStructureStatement() {
		ref<StructureStatement> statement = createRef<StructureStatement>();
		statement->identifierTokenIndex = tokenIndex;

		registerVariableSemanticToken(read(), true);

		const uint32 structureStatementIndex = static_cast<uint32>(output->ast.statements.size());

		output->registerCompletionItem(output->getTokenString(read()), CompletionItemKind::STRUCTURE);

		jump();
		jump();
		jumpEol();

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		parentStatementIndex = getLastStatementIndex();

		while (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE) {
			if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
				if (!parseAnnotationBlocks()) {
					return false;
				}
			}

			if (!parseMemberStatement()) {
				return false;
			}

			statement->memberStatementIndices.emplace_back(getLastStatementIndex());

			EXPECT(
				read().type == TokenType::RIGHT_CURLY_BRACE || read().type == TokenType::END_OF_FILE || read().type == TokenType::END_OF_LINE,
				"[FUNCTION] Expected '}' or EOF or EOL but got " + output->getTokenString(read()) + "."
			);

			jumpEol();
		}

		EXPECT(read().type == TokenType::RIGHT_CURLY_BRACE, "[STRUCTURE] Expected '}' but got EOF.");

		analyzer.analyzeStructureStatement(statement, read().getEnd());

		jump();

		return true;
	}

	bool Parser::parseMemberStatement() {
		ref<MemberStatement> statement = createRef<MemberStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationBlockIndexOffset = currentAnnotationCount;
		statement->annotationBlockCount = static_cast<uint32>(output->ast.annotationBlocks.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationBlockCount;
		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		statement->typeExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT2(read().type >= TokenType::USER_DEFINED, DiagnosticCode::MISSING_IDENTIFIER, read());

		statement->identifierTokenIndex = tokenIndex;

		const ref<StructureStatement> parentStatement = std::static_pointer_cast<StructureStatement>(output->ast.statements[parentStatementIndex]);

		analyzer.analyzeMemberStatement(parentStatement, statement);

		// NOTE: The identifier token index must be added AFTER the semantic analysis has been done on the member.
		parentStatement->memberIdentifierTokenIndices.emplace_back(tokenIndex);

		if (!registerVariableSemanticToken(read(), false)) {
			output->registerSemanticToken(tokenIndex, SemanticTokenType::MEMBER, 0);
		}

		jump();

		output->ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseConstantOrFunctionStatement() {
		ref<ConstantStatement> statement = createRef<ConstantStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationBlockIndexOffset = currentAnnotationCount;
		statement->annotationBlockCount = static_cast<uint32>(output->ast.annotationBlocks.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationBlockCount;
		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		if (peek().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionStatement();
		}

		statement->typeExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT2(read().type >= TokenType::USER_DEFINED, DiagnosticCode::MISSING_IDENTIFIER, read());

		statement->identifierTokenIndex = tokenIndex;

		output->registerCompletionItem(output->getTokenString(output->tokens[statement->identifierTokenIndex]), CompletionItemKind::CONSTANT);

		registerVariableSemanticToken(read(), false);

		jump();

		EXPECT(read().type == TokenType::EQUAL, "[CONSTANT] Expected '=' but got " + output->getTokenString(read()) + ".");

		output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

		jump();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		statement->initializerExpressionReferenceIndex = getLastExpressionReferenceIndex();
		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		analyzer.analyzeConstantStatement(statement);

		EXPECT(read().type == TokenType::END_OF_FILE || read().type == TokenType::END_OF_LINE, "[CONSTANT] Expected EOL but got " + output->getTokenString(read()) + ".");
		jumpEol();

		return true;
	}

	// TODO: A function return type should not be annotatable.
	bool Parser::parseFunctionStatement() {
		ref<FunctionStatement> statement = createRef<FunctionStatement>();
		statement->returnTypeExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[FUNCTION] Expected a UDI but got " + output->getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;

		output->registerSemanticToken(statement->identifierTokenIndex, SemanticTokenType::FUNCTION, 0);

		jump();
		jump();
		jumpEol();

		// TODO: Open a definition scope to register parameters.
		// output->openScope(position);

		while (read().type != TokenType::RIGHT_PARENTHESIS && read().type != TokenType::END_OF_FILE) {
			if (!parseParameterStatement()) {
				return false;
			}

			statement->parameterStatementIndices.emplace_back(getLastStatementIndex());

			if (read().type == TokenType::RIGHT_PARENTHESIS) {
				break;
			}

			EXPECT(read().type == TokenType::COMMA || read().type == TokenType::END_OF_LINE, "[FUNCTION] Expected ',' or EOL but got " + output->getTokenString(read()) + ".");

			jump();
			jumpEol();
		}

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[FUNCTION] Expected ')' but got " + output->getTokenString(read()) + ".");
		jump();

		EXPECT(read().type == TokenType::LEFT_CURLY_BRACE, "[FUNCTION] Expected '{' but got " + output->getTokenString(read()) + ".");

		statement->scopeStatementIndex = static_cast<uint32>(output->ast.statements.size());

		if (!parseScopeStatement()) {
			return false;
		}

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());
		// output->closeScope(position);
		output->registerCompletionItem(output->getTokenString(output->tokens[statement->identifierTokenIndex]), CompletionItemKind::FUNCTION);
		analyzer.analyzeFunctionStatement(getLastStatementIndex(), position);

		return true;
	}

	bool Parser::parseParameterStatement() {
		if (!parseTypeExpression()) {
			return false;
		}

		ref<ParameterStatement> statement = createRef<ParameterStatement>();
		statement->typeExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT2(read().type >= TokenType::USER_DEFINED, DiagnosticCode::MISSING_IDENTIFIER, read());

		statement->identifierTokenIndex = tokenIndex;

		// TODO: Register token depending on the case.
		output->registerSemanticToken(tokenIndex, SemanticTokenType::PARAMETER, 0);

		jump();

		output->ast.statements.emplace_back(statement);
		analyzer.analyzeParameterStatement(statement);

		return true;
	}

	bool Parser::parseLocalScopeStatement() {
		if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			if (!parseAnnotationBlocks()) {
				return false;
			}
		}

		if (read().type == TokenType::LEFT_CURLY_BRACE) {
			return parseScopeStatement();
		}

		if (read().type == TokenType::KEYWORD_IF) {
			return parseIfStatementStart();
		}

		if (read().type == TokenType::KEYWORD_RETURN) {
			return parseReturnStatement();
		}

		if (read().type == TokenType::KEYWORD_WHILE) {
			return parseWhileStatementStart();
		}

		if (visitedWhileCount > 0) {
			if (read().type == TokenType::KEYWORD_BREAK) {
				return parseBreakStatement();
			}

			if (read().type == TokenType::KEYWORD_NEXT) {
				return parseNextStatement();
			}
		}

		if (read().type >= TokenType::USER_DEFINED && ((peek().type >= TokenType::EQUAL && peek().type <= TokenType::VERTICAL_BAR_EQUAL) || peek().type == TokenType::DOT)) {
			return parseExpressionStatement();
		}

		return parseVariableStatement();
	}

	bool Parser::parseBreakStatement() {
		const ref<BreakStatement> statement = createRef<BreakStatement>();

		jump();

		EXPECT(read().type == TokenType::END_OF_LINE, "[NEXT] Expected EOL but got " + output->getTokenString(read()) + ".");

		output->ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseNextStatement() {
		const ref<NextStatement> statement = createRef<NextStatement>();

		jump();

		EXPECT(read().type == TokenType::END_OF_LINE, "[NEXT] Expected EOL but got " + output->getTokenString(read()) + ".");

		output->ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseExpressionStatement() {
		if (!parseAssignmentOrCompoundExpression()) {
			return false;
		}

		ref<ExpressionStatement> statement = createRef<ExpressionStatement>();
		statement->expressionReferenceIndex = getLastExpressionReferenceIndex();

		output->ast.statements.emplace_back(statement);

		EXPECT(read().type == TokenType::RIGHT_CURLY_BRACE || read().type == TokenType::END_OF_LINE, "[EXPRESSION] Expected '}' but got " + output->getTokenString(read()) + ".");

		return true;
	}

	bool Parser::parseIfStatementStart() {
		jump();

		ref<IfStatement> statement = createRef<IfStatement>();

		if (!parseIfOrWhileStatementEnd(statement->conditionExpressionReferenceIndex, statement->scopeStatementIndex)) {
			return false;
		}

		output->ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseWhileStatementStart() {
		jump();

		visitedWhileCount += 1;

		ref<WhileStatement> statement = createRef<WhileStatement>();

		if (!parseIfOrWhileStatementEnd(statement->conditionExpressionReferenceIndex, statement->scopeStatementIndex)) {
			return false;
		}

		output->ast.statements.emplace_back(statement);

		visitedWhileCount -= 1;

		return true;
	}

	bool Parser::parseScopeStatement() {
		ref<ScopeStatement> statement = createRef<ScopeStatement>();

		output->ast.statements.emplace_back(statement);

		jump();

		output->openScope(position);

		jumpEol();

		while (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE) {
			if (!parseLocalScopeStatement()) {
				return false;
			}

			statement->statementIndices.emplace_back(getLastStatementIndex());

			jumpEol();
		}

		EXPECT(read().type == TokenType::RIGHT_CURLY_BRACE, "[SCOPE] Expected '}' but got " + output->getTokenString(read()) + ".");
		jump();

		output->closeScope(position);

		return true;
	}

	bool Parser::parseIfOrWhileStatementEnd(uint32& conditionExpressionReferenceIndex, uint32& scopeStatementIndex) {
		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		conditionExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT(read().type == TokenType::LEFT_CURLY_BRACE, "[IF/WHILE] Expected '{' but got " + output->getTokenString(read()) + ".");

		scopeStatementIndex = getLastStatementIndex() + 1;

		if (!parseScopeStatement()) {
			return false;
		}

		return true;
	}

	bool Parser::parseReturnStatement() {
		jump();

		ref<ReturnStatement> statement = createRef<ReturnStatement>();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		statement->expressionReferenceIndex = getLastExpressionReferenceIndex();

		output->ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseVariableStatement() {
		if (!parseTypeExpression()) {
			return false;
		}

		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->typeExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[VARIABLE] Expected a UDI but got " + output->getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;

		registerVariableSemanticToken(read(), false);

		statement->constant = isScreamingSnakeCase(output->getIdentifier(read().type));

		uint32 modifiers = 0;

		if (statement->constant) {
			modifiers |= SemanticTokenModifier::CONSTANT;
		}

		output->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, modifiers);

		jump();

		if (statement->constant) {
			EXPECT(read().type == TokenType::EQUAL, "[VARIABLE/CONSTANT] Expected '=' but got " + output->getTokenString(read()) + ".");
		}

		if (read().type == TokenType::EQUAL) {
			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			statement->initializerExpressionReferenceIndex = getLastExpressionReferenceIndex();
		}

		output->ast.statements.emplace_back(statement);
		analyzer.analyzeVariableStatement(statement);

		return true;
	}

	bool Parser::parseTypeExpression() {
		const uint32 typeIndex = read().type;

		EXPECT(
			(typeIndex >= FIRST_TYPE_INDEX && typeIndex <= LAST_TYPE_INDEX) || typeIndex >= TokenType::USER_DEFINED,
			"[TYPE] Expected a language or user-defined type but got " + output->getTokenString(read()) + "."
		);

		uint32 templateArgumentIndex = 0;

		Expression<ExpressionType::TYPE> expression{};
		expression.typeTokenIndex = tokenIndex;

		if (read().type >= TokenType::USER_DEFINED) {
			registerVariableSemanticToken(read(), true);
		}

		if (peek().type == TokenType::LEFT_ANGLE_BRACKET) {
			jump();
			jump();

			templateArgumentIndex = read().type;

			EXPECT(
				(templateArgumentIndex >= FIRST_TYPE_INDEX && templateArgumentIndex <= LAST_TYPE_INDEX) || templateArgumentIndex >= TokenType::USER_DEFINED,
				"[TYPE] Expected a valid template argument but got " + output->getTokenString(read()) + "."
			);

			expression.templateArgumentTokenIndex = tokenIndex;

			if (read().type >= TokenType::USER_DEFINED) {
				registerVariableSemanticToken(read(), true);
			}

			jump();

			EXPECT(read().type == TokenType::RIGHT_ANGLE_BRACKET, "[TYPE] Expected '>' but got " + output->getTokenString(read()) + ".");
		}

		jump();

		expression.type = PackedType(typeIndex, templateArgumentIndex);

		analyzer.analyzeTypeExpression(expression);

		output->ast.addExpression(expression);

		return true;
	}

	bool Parser::parseAssignmentOrCompoundExpression() {
		EXPECT(read().type >= TokenType::USER_DEFINED, "[ASSIGNMENT] Expected a user-defined identifier but got " + output->getTokenString(read()) + ".");

		if (!parseIdentifierExpression()) {
			return false;
		}

		if (read().type >= TokenType::EQUAL && read().type <= TokenType::VERTICAL_BAR_EQUAL) {
			Expression<ExpressionType::BINARY> expression{};
			expression.leftExpressionReferenceIndex = getLastExpressionReferenceIndex();
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			if (!parseLogicalOrExpression()) {
				return false;
			}

			expression.rightExpressionReferenceIndex = getLastExpressionReferenceIndex();

			output->ast.addExpression(expression);
		}

		return true;
	}

	bool Parser::parseLogicalOrExpression() {
		BINARY_EXPRESSION(read().type == TokenType::KEYWORD_OR, parseLogicalXorExpression);
	}

	bool Parser::parseLogicalXorExpression() {
		BINARY_EXPRESSION(read().type == TokenType::KEYWORD_XOR, parseLogicalAndExpression);
	}

	bool Parser::parseLogicalAndExpression() {
		BINARY_EXPRESSION(read().type == TokenType::KEYWORD_AND, parseBitwiseOrExpression);
	}

	bool Parser::parseBitwiseOrExpression() {
		BINARY_EXPRESSION(read().type == TokenType::VERTICAL_BAR, parseBitwiseXorExpression);
	}

	bool Parser::parseBitwiseXorExpression() {
		BINARY_EXPRESSION(read().type == TokenType::CARET, parseBitwiseAndExpression);
	}

	bool Parser::parseBitwiseAndExpression() {
		BINARY_EXPRESSION(read().type == TokenType::AMPERSAND, parseEqualityExpression);
	}

	bool Parser::parseEqualityExpression() {
		BINARY_EXPRESSION(read().type == TokenType::EQUAL_EQUAL || read().type == TokenType::EXCLAMATION_EQUAL, parseRelationalExpression);
	}

	bool Parser::parseRelationalExpression() {
		BINARY_EXPRESSION(read().type >= TokenType::LEFT_ANGLE_BRACKET && read().type <= TokenType::RIGHT_ANGLE_BRACKET_EQUAL, parseBitwiseShiftExpression);
	}

	bool Parser::parseBitwiseShiftExpression() {
		BINARY_EXPRESSION(read().type == TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET || read().type == TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET, parseAdditiveExpression);
	}

	bool Parser::parseAdditiveExpression() {
		BINARY_EXPRESSION(read().type == TokenType::MINUS || read().type == TokenType::PLUS, parseMultiplicativeExpression);
	}

	bool Parser::parseMultiplicativeExpression() {
		BINARY_EXPRESSION(read().type == TokenType::ASTERISK || read().type == TokenType::SLASH, parseUnaryExpression);
	}

	bool Parser::parseUnaryExpression() {
		if (read().type >= TokenType::PLUS && read().type <= TokenType::TILDE) {
			Expression<ExpressionType::UNARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			if (!parsePrimaryExpression()) {
				return false;
			}

			expression.rightExpressionReferenceIndex = getLastExpressionReferenceIndex();

			output->ast.addExpression(expression);

			return true;
		}

		return parsePrimaryExpression();
	}

	bool Parser::parsePrimaryExpression() {
		if (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX) {
			if (!parseTypeExpression()) {
				return false;
			}

			EXPECT(read().type == TokenType::LEFT_PARENTHESIS, "[PRIMARY] Expected '(' but got " + output->getTokenString(read()) + ".");

			return parseFunctionCallOrGroupExpression();
		}

		if (read().type >= TokenType::USER_DEFINED || (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX)) {
			if (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX) {
				output->registerSemanticToken(tokenIndex, SemanticTokenType::FUNCTION, SemanticTokenModifier::PREDEFINED);
			}

			if (!parseIdentifierExpression()) {
				return false;
			}

			if (read().type == TokenType::LEFT_PARENTHESIS) {
				return parseFunctionCallOrGroupExpression();
			}

			return true;
		}

		if (read().type >= TokenType::LITERAL_FLOATING_POINT && read().type <= TokenType::LITERAL_INTEGER_BASE_16) {
			return parseLiteralExpression();
		}

		if (read().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionCallOrGroupExpression();
		}

		generateDiagnostic<DiagnosticCode::MISSING_EXPRESSION>(*output, read());

		return false;
	}

	bool Parser::parseIdentifierExpression() {
		Expression<ExpressionType::IDENTIFIER> expression{};
		expression.tokenIndex = tokenIndex;

		if (isScreamingSnakeCase(output->getIdentifier(read().type))) {
			output->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);
		}

		jump();

		output->ast.addExpression(expression);

		analyzer.analyzeIdentifierExpression(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseMemberAccessExpression() {
		jump();

		Expression<ExpressionType::MEMBER_ACCESS> expression{};
		expression.leftExpressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT2(read().type >= TokenType::USER_DEFINED, DiagnosticCode::MISSING_IDENTIFIER, read());

		expression.identifierTokenIndex = tokenIndex;

		jump();

		output->ast.addExpression(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseFunctionCallOrGroupExpression() {
		const uint32 lastExpressionReferenceIndex = getLastExpressionReferenceIndex();
		const ExpressionReference& lastExpressionReference = output->ast.getExpressionReference(lastExpressionReferenceIndex);

		if (lastExpressionReference.type != ExpressionType::TYPE && lastExpressionReference.type != ExpressionType::IDENTIFIER) {
			return parseGroupExpression();
		}

		Expression<ExpressionType::FUNCTION_CALL> expression{};
		expression.calleeExpressionReferenceIndex = lastExpressionReferenceIndex;

		jump();

		while (read().type != TokenType::RIGHT_PARENTHESIS && read().type != TokenType::END_OF_FILE) {
			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			expression.argumentExpressionReferenceIndices.emplace_back(getLastExpressionReferenceIndex());

			if (read().type == TokenType::RIGHT_PARENTHESIS) {
				break;
			}

			EXPECT(read().type == TokenType::COMMA, "[FUNCTION_CALL] Expected ',' but got " + output->getTokenString(read()) + ".");

			jump();
		}

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[FUNCTION_CALL] Expected ')' but got " + output->getTokenString(read()) + ".");

		jump();

		output->ast.addExpression(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseLiteralExpression() {
		Expression<ExpressionType::LITERAL> expression{};
		expression.tokenIndex = tokenIndex;

		jump();

		output->ast.addExpression(expression);

		return true;
	}

	bool Parser::parseGroupExpression() {
		jump();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		Expression<ExpressionType::GROUP> expression{};
		expression.expressionReferenceIndex = getLastExpressionReferenceIndex();

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[GROUP] Expected ')' but got " + output->getTokenString(read()) + ".");

		jump();

		output->ast.addExpression(expression);

		return true;
	}

	bool Parser::parseAnnotationBlocks() {
		while (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			jump();

			// Annotations are considered user-defined identifiers because they can be used to name variables.
			EXPECT(read().type >= TokenType::USER_DEFINED, "[ANNOTATION_INSTANCE] Expected an annotation but got " + output->getTokenString(read()) + ".");

			AnnotationBlock annotationBlock{};

			for (uint32 annotationIndex = 0; annotationIndex < ANNOTATION_COUNT; annotationIndex += 1) {
				const char8* annotation = ANNOTATION_STRINGS[annotationIndex];

				if (output->getTokenString(read()) == annotation) {
					annotationFlag |= BIT(annotationIndex);
					annotationBlock.annotation = static_cast<Annotation>(BIT(annotationIndex));

					break;
				}
			}

			annotationBlock.identifierTokenIndex = tokenIndex;

			if (annotationBlock.annotation == static_cast<Annotation>(0)) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, read());
			}
			else {
				output->registerSemanticToken(annotationBlock.identifierTokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::PREDEFINED);
			}

			jump();

			if (!(ANNOTATION_TYPE_VOID & annotationBlock.annotation)) {
				if (read().type < TokenType::LITERAL_FLOATING_POINT || read().type > TokenType::LITERAL_INTEGER_BASE_16) {
					const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

					// TODO
					/* output->generateDiagnostic(
						DiagnosticSeverity::ERROR,
						"[ANNOTATION_INSTANCE] Expected a literal but got " + output->getTokenString(read()) + ".",
						identifierToken.getEnd(),
						identifierToken.getEnd()
					); */
				}
				else {
					annotationBlock.valueTokenIndex = tokenIndex;

					output->registerSemanticToken(tokenIndex, SemanticTokenType::NUMBER, 0);

					jump();
				}
			}

			EXPECT(read().type == TokenType::RIGHT_SQUARE_BRACKET, "[ANNOTATION_INSTANCE] Expected ']' but got " + output->getTokenString(read()) + ".");

			jump();

			output->ast.annotationBlocks.emplace_back(annotationBlock);

			jumpEol();
		}

		return true;
	}

	uint32 Parser::getLastStatementIndex() const {
		DEBUG_ASSERT(!output->ast.statements.empty());

		return static_cast<uint32>(output->ast.statements.size()) - 1;
	}

	uint32 Parser::getLastExpressionReferenceIndex() const {
		DEBUG_ASSERT(!output->ast.expressionReferences.empty());

		return static_cast<uint32>(output->ast.expressionReferences.size()) - 1;
	}

	Token Parser::read() const {
		return output->tokens[tokenIndex];
	}

	Token Parser::peek() const {
		if (tokenIndex + 1 >= output->tokens.size()) {
			return output->tokens.back();
		}

		return output->tokens[tokenIndex + 1];
	}

	void Parser::jump() {
		if (read().type == TokenType::END_OF_FILE) {
			return;
		}

		position = read().start;
		position.characterOffset += read().characterCount;

		tokenIndex += 1;
	}

	void Parser::jumpEol() {
		while (read().type == TokenType::END_OF_LINE) {
			jump();
		}
	}

	void Parser::synchronize() {
		while (read().type != TokenType::END_OF_LINE && read().type != TokenType::END_OF_FILE) {
			jump();
		}
	}

	bool Parser::registerVariableSemanticToken(const Token& token, bool preferStructure) const {
		const std::string& identifier = output->getIdentifier(token.type);

		// TODO: Refactor.
		if (preferStructure) {
			if (isPascalCase(identifier)) {
				output->registerSemanticToken(token, SemanticTokenType::STRUCTURE, 0);

				return true;
			}

			if (isScreamingSnakeCase(identifier)) {
				output->registerSemanticToken(token, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

				return true;
			}
		}
		else {
			if (isScreamingSnakeCase(identifier)) {
				output->registerSemanticToken(token, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

				return true;
			}

			if (isPascalCase(identifier)) {
				output->registerSemanticToken(token, SemanticTokenType::STRUCTURE, 0);

				return true;
			}
		}

		return false;
	}
}