#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Parser.hpp"

namespace ProjectA {
	#define EXPECT(condition, message) \
		if (!(condition)) { \
			compilationResult->generateDiagnostic(DiagnosticSeverity::ERROR, message, tokenIndex); \
			\
			return false; \
		}

	#define BINARY_EXPRESSION(condition, callback) \
		if (!callback()) { \
			return false; \
		} \
		\
		while (condition) { \
			ref<BinaryExpression> expression = createRef<BinaryExpression>(); \
			expression->leftExpressionIndex = getLastExpressionIndex(); \
			expression->operatorTokenIndex = tokenIndex; \
			\
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0); \
			\
			jump(); \
			\
			if (!callback()) { \
				return false; \
			} \
			\
			expression->rightExpressionIndex = getLastExpressionIndex(); \
			\
			ast.expressions.emplace_back(expression); \
		} \
		\
		return true;

	AST Parser::parse(CompilationResult& compilationResult) {
		this->compilationResult = &compilationResult;
		ast = {};
		tokenIndex = 0;
		currentAnnotationCount = 0;
		annotationFlag = 0;
		position = {};
		parentStatementIndex = ~0;

		analyzer.setCompilationResult(compilationResult);
		analyzer.setAst(ast);

		skipEmptyLines();

		while (read().type != TokenType::END_OF_FILE) {
			if (!parseGlobalScopeStatement()) {
				synchronize();
			}

			skipEmptyLines();
		}

		return ast;
	}

	bool Parser::parseGlobalScopeStatement() {
		if (read().type >= TokenType::USER_DEFINED && peek().type == TokenType::LEFT_CURLY_BRACE) {
			// A structure statement must start with a UDI followed by '{'.
			return parseStructureStatement();
		}

		if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			if (!parseAnnotationInstances()) {
				return false;
			}
		}

		// The constant and function statements can either start with a UDI, language type or '[' (annotation).
		return parseConstantOrFunctionStatement();
	}

	bool Parser::parseStructureStatement() {
		ref<StructureStatement> statement = createRef<StructureStatement>();
		statement->identifierTokenIndex = tokenIndex;

		EXPECT(isPascalCase(read().value.c_str()), "[STRUCTURE] Structure name is not in PascalCase.");

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::STRUCTURE, 0);

		jump();
		jump();

		skipEmptyLines();

		ast.statements.emplace_back(statement);
		ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		parentStatementIndex = getLastStatementIndex();

		while (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE) {
			if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
				if (!parseAnnotationInstances()) {
					return false;
				}
			}

			if (!parseMemberStatement()) {
				return false;
			}

			statement->memberStatementIndices.emplace_back(getLastStatementIndex());

			EXPECT(
				read().type == TokenType::RIGHT_CURLY_BRACE || read().type == TokenType::END_OF_FILE || read().type == TokenType::END_OF_LINE,
				"[FUNCTION] Expected '}' or EOF or EOL but got " + getTokenString(read()) + "."
			);

			skipEmptyLines();
		}

		EXPECT(read().type != TokenType::END_OF_FILE, "[STRUCTURE] Expected '}' but got EOF.");

		jump();

		analyzer.analyzeStructureStatement(statement);

		// TODO: Update UDT here, e.g.
		// ast.userDefinedTypes.emplace_back(compilationResult->tokens[statement->identifierTokenIndex].value);

		return true;
	}

	bool Parser::parseMemberStatement() {
		ref<StructureMemberStatement> statement = createRef<StructureMemberStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationInstanceIndexOffset = currentAnnotationCount;
		statement->annotationInstanceCount = static_cast<uint32>(ast.annotationInstances.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationInstanceCount;
		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		statement->typeExpressionIndex = getLastExpressionIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[MEMBER] Expected an identifier but got " + getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;

		const ref<StructureStatement> parentStatement = std::static_pointer_cast<StructureStatement>(ast.statements[parentStatementIndex]);

		analyzer.analyzeMemberStatement(parentStatement, statement);

		// NOTE: The identifier token index must be added AFTER the semantic analysis has been done on the member.
		parentStatement->memberIdentifierTokenIndices.emplace_back(tokenIndex);

		if (!registerVariableSemanticToken(read())) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::MEMBER, 0);
		}

		jump();

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseConstantOrFunctionStatement() {
		ref<GlobalScopeConstantStatement> statement = createRef<GlobalScopeConstantStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationInstanceIndexOffset = currentAnnotationCount;
		statement->annotationInstanceCount = static_cast<uint32>(ast.annotationInstances.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationInstanceCount;
		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		if (peek().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionStatement();
		}

		statement->typeExpressionIndex = getLastExpressionIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[CONSTANT] Expected an identifier but got " + getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;

		EXPECT(isScreamingSnakeCase(read().value.c_str()), "[CONSTANT] Constant identifier " + getTokenString(read()) + " is not written in SCREAMING_SNAKE_CASE.");

		jump();

		EXPECT(read().type == TokenType::EQUAL, "[CONSTANT] Expected '=' but got " + getTokenString(read()) + ".");

		compilationResult->registerSemanticToken(statement->identifierTokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);
		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

		jump();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		statement->initializerExpressionIndex = getLastExpressionIndex();
		ast.statements.emplace_back(statement);
		ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		EXPECT(read().type == TokenType::END_OF_FILE || read().type == TokenType::END_OF_LINE, "[CONSTANT] Expected EOL but got " + getTokenString(read()) + ".");

		skipEmptyLines();

		return true;
	}

	bool Parser::parseFunctionStatement() {
		// TODO: A function return type cannot be annotated.

		ref<FunctionStatement> statement = createRef<FunctionStatement>();
		statement->returnTypeExpressionIndex = getLastExpressionIndex();
		statement->identifierTokenIndex = tokenIndex;

		EXPECT(read().type >= TokenType::USER_DEFINED, "[FUNCTION] Expected a UDI but got " + getTokenString(read()) + ".");

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::FUNCTION, 0);

		jump();
		jump();
		skipEmptyLines();

		while (read().type != TokenType::RIGHT_PARENTHESIS && read().type != TokenType::END_OF_FILE) {
			if (!parseParameterStatement()) {
				return false;
			}

			statement->parameterStatementIndices.emplace_back(getLastStatementIndex());

			if (read().type == TokenType::RIGHT_PARENTHESIS) {
				break;
			}

			EXPECT(read().type == TokenType::COMMA || read().type == TokenType::END_OF_LINE, "[FUNCTION] Expected ',' or EOL but got " + getTokenString(read()) + ".");

			jump();
			skipEmptyLines();
		}

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[FUNCTION] Expected ')' but got " + getTokenString(read()) + ".");

		jump();

		EXPECT(read().type == TokenType::LEFT_CURLY_BRACE, "[FUNCTION] Expected '{' but got " + getTokenString(read()) + ".");

		if (!parseScopeStatement()) {
			return false;
		}

		statement->scopeStatementIndex = getLastStatementIndex();

		ast.statements.emplace_back(statement);
		ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		return true;
	}

	bool Parser::parseParameterStatement() {
		if (!parseTypeExpression()) {
			return false;
		}

		ref<ParameterStatement> statement = createRef<ParameterStatement>();
		statement->typeExpressionIndex = getLastExpressionIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[PARAMETER] Expected an identifier but got " + getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::PARAMETER, 0);

		jump();

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseLocalScopeStatement() {
		if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			if (!parseAnnotationInstances()) {
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

		if (read().type >= TokenType::USER_DEFINED && ((peek().type >= TokenType::EQUAL && peek().type <= TokenType::VERTICAL_BAR_EQUAL) || peek().type == TokenType::DOT)) {
			return parseExpressionStatement();
		}

		return parseVariableStatement();
	}

	bool Parser::parseExpressionStatement() {
		if (!parseAssignmentOrCompoundExpression()) {
			return false;
		}

		ref<ExpressionStatement> statement = createRef<ExpressionStatement>();
		statement->expressionIndex = getLastExpressionIndex();

		ast.statements.emplace_back(statement);

		EXPECT(read().type == TokenType::RIGHT_CURLY_BRACE || read().type == TokenType::END_OF_LINE, "[EXPRESSION] Expected '}' but got " + getTokenString(read()) + ".");

		return true;
	}

	bool Parser::parseIfStatementStart() {
		jump();

		ref<IfStatement> statement = createRef<IfStatement>();

		if (!parseIfOrWhileStatementEnd(statement->conditionExpressionIndex, statement->scopeStatementIndex)) {
			return false;
		}

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseWhileStatementStart() {
		jump();

		ref<WhileStatement> statement = createRef<WhileStatement>();

		if (!parseIfOrWhileStatementEnd(statement->conditionExpressionIndex, statement->scopeStatementIndex)) {
			return false;
		}

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseScopeStatement() {
		ref<ScopeStatement> statement = createRef<ScopeStatement>();

		jump();
		skipEmptyLines();

		while (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE) {
			if (!parseLocalScopeStatement()) {
				return false;
			}

			statement->statementIndices.emplace_back(getLastStatementIndex());

			skipEmptyLines();
		}

		EXPECT(read().type == TokenType::RIGHT_CURLY_BRACE, "[SCOPE] Expected '}' but got " + getTokenString(read()) + ".");

		jump();

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseIfOrWhileStatementEnd(uint32& conditionExpressionIndex, uint32& scopeStatementIndex) {
		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		conditionExpressionIndex = getLastExpressionIndex();

		EXPECT(read().type == TokenType::LEFT_CURLY_BRACE, "[IF/WHILE] Expected '{' but got " + getTokenString(read()) + ".");

		if (!parseScopeStatement()) {
			return false;
		}

		scopeStatementIndex = getLastStatementIndex();

		return true;
	}

	bool Parser::parseReturnStatement() {
		jump();

		ref<ReturnStatement> statement = createRef<ReturnStatement>();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		statement->expressionIndex = getLastExpressionIndex();

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseVariableStatement() {
		if (!parseTypeExpression()) {
			return false;
		}

		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->typeExpressionIndex = getLastExpressionIndex();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[VARIABLE] Expected a UDI but got " + getTokenString(read()) + ".");

		statement->identifierTokenIndex = tokenIndex;
		statement->constant = isScreamingSnakeCase(read().value.c_str());

		if (statement->constant) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);
		}
		else {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, 0);
		}

		jump();

		if (statement->constant) {
			EXPECT(read().type == TokenType::EQUAL, "[VARIABLE/CONSTANT] Expected '=' but got " + getTokenString(read()) + ".");
		}

		if (read().type == TokenType::EQUAL) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			statement->initializerExpressionIndex = getLastExpressionIndex();
		}

		ast.statements.emplace_back(statement);

		return true;
	}

	bool Parser::parseTypeExpression() {
		EXPECT(
			(read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX) || read().type >= TokenType::USER_DEFINED,
			"[TYPE] Expected a language or user-defined type but got " + getTokenString(read()) + "."
		);

		ref<TypeExpression> expression = createRef<TypeExpression>();
		expression->typeTokenIndex = tokenIndex;

		if (read().type >= TokenType::USER_DEFINED) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::STRUCTURE, 0);
		}

		if (peek().type == TokenType::LEFT_ANGLE_BRACKET) {
			EXPECT(read().type < TokenType::USER_DEFINED, "[TYPE] Cannot apply template argument to user-defined identifier " + getTokenString(read()) + ".");

			jump();
			jump();

			EXPECT(read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX, "[TYPE] Expected a valid template argument but got " + getTokenString(read()) + ".");

			expression->templateArgumentTokenIndex = tokenIndex;

			jump();

			EXPECT(read().type == TokenType::RIGHT_ANGLE_BRACKET, "[TYPE] Expected '>' but got " + getTokenString(read()) + ".");
		}

		jump();

		ast.expressions.emplace_back(expression);

		return true;
	}

	bool Parser::parseAssignmentOrCompoundExpression() {
		EXPECT(read().type >= TokenType::USER_DEFINED, "[ASSIGNMENT] Expected an user-defined identifier but got " + getTokenString(read()) + ".");

		if (!parseIdentifierExpression()) {
			return false;
		}

		if (read().type >= TokenType::EQUAL && read().type <= TokenType::VERTICAL_BAR_EQUAL) {
			ref<BinaryExpression> expression = createRef<BinaryExpression>();
			expression->leftExpressionIndex = getLastExpressionIndex();
			expression->operatorTokenIndex = tokenIndex;

			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			if (!parseLogicalOrExpression()) {
				return false;
			}

			expression->rightExpressionIndex = getLastExpressionIndex();

			ast.expressions.emplace_back(expression);
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
			ref<UnaryExpression> expression = createRef<UnaryExpression>();
			expression->operatorTokenIndex = tokenIndex;

			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			if (!parsePrimaryExpression()) {
				return false;
			}

			expression->rightExpressionIndex = getLastExpressionIndex();

			ast.expressions.emplace_back(expression);

			return true;
		}

		return parsePrimaryExpression();
	}

	bool Parser::parsePrimaryExpression() {
		if (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX) {
			if (!parseTypeExpression()) {
				return false;
			}

			EXPECT(read().type == TokenType::LEFT_PARENTHESIS, "[PRIMARY] Expected '(' but got " + getTokenString(read()) + ".");

			return parseFunctionCallOrGroupExpression();
		}

		if (read().type >= TokenType::USER_DEFINED || (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX)) {
			if (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX) {
				compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::FUNCTION, SemanticTokenModifier::PREDEFINED);
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
			return parseNumberExpression();
		}

		if (read().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionCallOrGroupExpression();
		}

		compilationResult->generateDiagnostic(DiagnosticSeverity::ERROR, "[PRIMARY] Expected an expression but got " + getTokenString(read()) + ".", tokenIndex);

		return false;
	}

	bool Parser::parseIdentifierExpression() {
		ref<IdentifierExpression> expression = createRef<IdentifierExpression>();
		expression->identifierTokenIndex = tokenIndex;

		if (isScreamingSnakeCase(read().value.c_str())) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);
		}

		jump();

		ast.expressions.emplace_back(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseMemberAccessExpression() {
		jump();

		EXPECT(read().type >= TokenType::USER_DEFINED, "[MEMBER_ACCESS] Expected an identifier but got " + getTokenString(read()) + ".");

		ref<MemberAccessExpression> expression = createRef<MemberAccessExpression>();
		expression->leftExpressionIndex = getLastExpressionIndex();
		expression->memberIdentifier = read().value;

		jump();

		ast.expressions.emplace_back(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseFunctionCallOrGroupExpression() {
		ref<FunctionCallExpression> expression = createRef<FunctionCallExpression>();

		const uint32 lastExpressionIndex = getLastExpressionIndex();
		const ref<Expression> lastExpression = ast.expressions[lastExpressionIndex];

		if (lastExpression->type != ExpressionType::TYPE && lastExpression->type != ExpressionType::IDENTIFIER) {
			return parseGroupExpression();
		}

		expression->calleeExpressionIndex = lastExpressionIndex;

		jump();

		while (read().type != TokenType::RIGHT_PARENTHESIS && read().type != TokenType::END_OF_FILE) {
			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			expression->argumentExpressionIndices.emplace_back(getLastExpressionIndex());

			if (read().type == TokenType::RIGHT_PARENTHESIS) {
				break;
			}

			EXPECT(read().type == TokenType::COMMA, "[FUNCTION_CALL] Expected ',' but got " + getTokenString(read()) + ".");

			jump();
		}

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[FUNCTION_CALL] Expected ')' but got " + getTokenString(read()) + ".");

		jump();

		ast.expressions.emplace_back(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseGroupExpression() {
		jump();

		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		ref<GroupExpression> expression = createRef<GroupExpression>();
		expression->expressionIndex = getLastExpressionIndex();

		EXPECT(read().type == TokenType::RIGHT_PARENTHESIS, "[GROUP] Expected ')' but got " + getTokenString(read()) + ".");

		jump();

		ast.expressions.emplace_back(expression);

		return true;
	}

	bool Parser::parseNumberExpression() {
		ref<NumberExpression> expression = createRef<NumberExpression>();
		expression->numberTokenIndex = tokenIndex;

		jump();

		ast.expressions.emplace_back(expression);

		return true;
	}

	bool Parser::parseAnnotationInstances() {
		while (read().type == TokenType::LEFT_SQUARE_BRACKET) {
			jump();

			// Annotations are considered user-defined identifiers because they can be used to name variables.
			EXPECT(read().type >= TokenType::USER_DEFINED, "[ANNOTATION_INSTANCE] Expected an annotation but got " + getTokenString(read()) + ".");

			AnnotationInstance annotationInstance{};

			for (uint32 annotationIndex = 0; annotationIndex < ANNOTATION_COUNT; annotationIndex += 1) {
				const char8* annotation = ANNOTATION_STRINGS[annotationIndex];

				if (read().value == annotation) {
					annotationFlag |= BIT(annotationIndex);
					annotationInstance.annotation = static_cast<Annotation>(BIT(annotationIndex));

					break;
				}
			}

			EXPECT(annotationInstance.annotation != static_cast<Annotation>(0), "[ANNOTATION_INSTANCE] Unknown annotation " + getTokenString(read()) + ".");

			annotationInstance.identifier = read();

			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::ANNOTATION, 0);

			jump();

			// The value can be provided either as a literal or as a constant identifier.
			if (read().type >= TokenType::LITERAL_FLOATING_POINT) {
				EXPECT(
					read().type < TokenType::USER_DEFINED || (read().type >= TokenType::USER_DEFINED && isScreamingSnakeCase(read().value.c_str())),
					"[ANNOTATION_INSTANCE] Constant identifier " + getTokenString(read()) + " is not written in screaming snake case."
				);

				annotationInstance.value = read();

				compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::NUMBER, 0);

				jump();
			}

			if (read().type != TokenType::RIGHT_SQUARE_BRACKET) {
				if (annotationInstance.value.value.empty()) {
					compilationResult->generateDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Expected a literal or ']' but got " + getTokenString(read()) + ".", tokenIndex);
				}
				else {
					compilationResult->generateDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Expected ']' but got " + getTokenString(read()) + ".", tokenIndex);
				}

				return false;
			}

			jump();

			ast.annotationInstances.emplace_back(annotationInstance);

			skipEmptyLines();
		}

		return true;
	}

	uint32 Parser::getLastStatementIndex() const {
		if (ast.statements.empty()) {
			return ~0;
		}

		return static_cast<uint32>(ast.statements.size()) - 1;
	}

	uint32 Parser::getLastExpressionIndex() const {
		if (ast.expressions.empty()) {
			return ~0;
		}

		return static_cast<uint32>(ast.expressions.size()) - 1;
	}

	Position Parser::getPosition() const {
		return position;
	}

	Token Parser::read() const {
		return compilationResult->tokens[tokenIndex];
	}

	Token Parser::peek() const {
		if (tokenIndex + 1 >= compilationResult->tokens.size()) {
			return compilationResult->tokens.back();
		}

		return compilationResult->tokens[tokenIndex + 1];
	}

	void Parser::jump() {
		if (read().type == TokenType::END_OF_FILE) {
			return;
		}

		position = read().start;
		position.characterOffset += read().characterCount;

		tokenIndex += 1;
	}

	void Parser::skipEmptyLines() {
		while (read().type == TokenType::END_OF_LINE) {
			tokenIndex += 1;
		}
	}

	void Parser::synchronize() {
		while (read().type != TokenType::END_OF_FILE) {
			skipEmptyLines();

			/* switch (read().type) {
				case TokenType::KEYWORD_IF:
				case TokenType::KEYWORD_WHILE:
				case TokenType::KEYWORD_RETURN: {
					return;
				}
				default: {
					break;
				}
			} */

			jump();
		}
	}

	bool Parser::registerVariableSemanticToken(const Token& token) const {
		const char8* value = token.value.c_str();

		if (isScreamingSnakeCase(value)) {
			compilationResult->registerSemanticToken(token, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

			return true;
		}

		if (isPascalCase(value)) {
			compilationResult->registerSemanticToken(token, SemanticTokenType::STRUCTURE, 0);

			return true;
		}

		return false;
	}
}