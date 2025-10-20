#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Parser.hpp"

namespace ProjectA {
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
		annotationFlag = 0;
		position = {};

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

		if (!isPascalCase(read().value.c_str())) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[STRUCTURE] Structure name is not in PascalCase.", tokenIndex);

			return false;
		}

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::STRUCTURE, 0);

		jump();
		jump();

		skipEmptyLines();

		while (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE) {
			if (read().type == TokenType::LEFT_SQUARE_BRACKET) {
				if (!parseAnnotationInstances()) {
					return false;
				}
			}

			if (!parseMemberOrParameterStatement()) {
				return false;
			}

			statement->memberStatementIndices.emplace_back(getLastStatementIndex());

			if (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_FILE && read().type != TokenType::END_OF_LINE) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION] Expected '}' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			skipEmptyLines();
		}

		if (read().type == TokenType::END_OF_FILE) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[STRUCTURE] Expected '}' but got EOF.", tokenIndex);

			return false;
		}

		jump();

		ast.statements.emplace_back(statement);
		ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		// TODO: Update UDT here, e.g.
		// ast.userDefinedTypes.emplace_back(compilationResult->tokens[statement->identifierTokenIndex].value);

		return true;
	}

	bool Parser::parseConstantOrFunctionStatement() {
		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationInstanceIndexOffset = static_cast<uint32>(ast.annotationInstances.size());
		statement->annotationInstanceCount = static_cast<uint32>(ast.annotationInstances.size()) - statement->annotationInstanceIndexOffset;

		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		statement->typeExpressionIndex = getLastExpressionIndex();

		if (read().type < TokenType::USER_DEFINED) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[CONSTANT] Expected an identifier but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		statement->identifierTokenIndex = tokenIndex;

		if (isScreamingSnakeCase(read().value.c_str())) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

			jump();

			if (read().type != TokenType::EQUAL) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[CONSTANT] Expected '=' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			statement->initializerOperatorTokenIndex = tokenIndex;

			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			statement->initializerExpressionIndex = getLastExpressionIndex();
			ast.statements.emplace_back(statement);
			ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

			if (read().type != TokenType::END_OF_LINE && read().type != TokenType::END_OF_FILE) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[CONSTANT] Expected EOL but got " + getTokenString(read()) + ".", tokenIndex);
			}
			else {
				skipEmptyLines();
			}

			return true;
		}

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::FUNCTION, 0);

		jump();

		if (read().type != TokenType::LEFT_PARENTHESIS) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[CONSTANT/FUNCTION] Expected '(' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		jump();

		return parseFunctionStatement(statement);
	}

	bool Parser::parseFunctionStatement(const ref<VariableStatement>& constantStatement) {
		ref<FunctionStatement> statement = createRef<FunctionStatement>();
		statement->returnTypeExpressionIndex = constantStatement->typeExpressionIndex;
		statement->identifierTokenIndex = constantStatement->identifierTokenIndex;

		while (read().type != TokenType::RIGHT_PARENTHESIS && read().type != TokenType::END_OF_FILE) {
			if (!parseMemberOrParameterStatement()) {
				return false;
			}

			statement->parameterStatementIndices.emplace_back(getLastStatementIndex());

			if (read().type == TokenType::RIGHT_PARENTHESIS) {
				break;
			}

			if (read().type != TokenType::COMMA) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION] Expected ',' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			jump();
		}

		if (read().type != TokenType::RIGHT_PARENTHESIS) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION] Expected ')' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		jump();

		if (read().type != TokenType::LEFT_CURLY_BRACE) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION] Expected '{' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		if (!parseScopeStatement()) {
			return false;
		}

		statement->scopeStatementIndex = getLastStatementIndex();

		ast.statements.emplace_back(statement);
		ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		return true;
	}

	bool Parser::parseMemberOrParameterStatement() {
		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationInstanceIndexOffset = static_cast<uint32>(ast.annotationInstances.size());
		statement->annotationInstanceCount = static_cast<uint32>(ast.annotationInstances.size()) - statement->annotationInstanceIndexOffset;

		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		statement->typeExpressionIndex = getLastExpressionIndex();

		if (read().type < TokenType::USER_DEFINED) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[PARAMETER] Expected an identifier but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		statement->identifierTokenIndex = tokenIndex;

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

		if (read().type != TokenType::RIGHT_CURLY_BRACE && read().type != TokenType::END_OF_LINE) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[EXPRESSION] Expected '}' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

		if (read().type != TokenType::RIGHT_CURLY_BRACE) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[SCOPE] Expected '}' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

		if (read().type != TokenType::LEFT_CURLY_BRACE) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[IF/WHILE] Expected '{' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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
		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationInstanceIndexOffset = static_cast<uint32>(ast.annotationInstances.size());
		statement->annotationInstanceCount = static_cast<uint32>(ast.annotationInstances.size()) - statement->annotationInstanceIndexOffset;

		annotationFlag = 0;

		if (!parseTypeExpression()) {
			return false;
		}

		statement->typeExpressionIndex = getLastExpressionIndex();

		if (read().type < TokenType::USER_DEFINED) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[VARIABLE] Expected a name identifier but got " + getTokenString(read()) + ".", tokenIndex);

			jump();

			return false;
		}

		const bool constant = isScreamingSnakeCase(read().value.c_str());

		statement->identifierTokenIndex = tokenIndex;

		compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, constant ? SemanticTokenModifier::CONSTANT : 0);

		jump();

		if (constant && read().type != TokenType::EQUAL) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[VARIABLE/CONSTANT] Expected '=' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		if (read().type == TokenType::EQUAL) {
			statement->initializerOperatorTokenIndex = tokenIndex;

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
		ref<TypeExpression> expression = createRef<TypeExpression>();

		if (read().type < TokenType::USER_DEFINED && (read().type < FIRST_TYPE_INDEX || read().type > LAST_TYPE_INDEX)) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[TYPE] Expected a valid type but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

		expression->typeTokenIndex = tokenIndex;

		if (read().type >= TokenType::USER_DEFINED) {
			compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::STRUCTURE, 0);
		}

		if (peek().type == TokenType::LEFT_ANGLE_BRACKET) {
			if (read().type >= TokenType::USER_DEFINED) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[TYPE] Cannot apply template argument to user-defined identifier " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			jump();
			jump();

			if (read().type < FIRST_TYPE_INDEX || read().type > LAST_TYPE_INDEX) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[TYPE] Expected a valid template argument but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			expression->templateArgumentTokenIndex = tokenIndex;

			jump();

			if (read().type != TokenType::RIGHT_ANGLE_BRACKET) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[TYPE] Expected '>' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}
		}

		jump();

		ast.expressions.emplace_back(expression);

		return true;
	}

	bool Parser::parseAssignmentOrCompoundExpression() {
		if (read().type < TokenType::USER_DEFINED) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ASSIGNMENT] Expected an user-defined identifier but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

			if (read().type != TokenType::LEFT_PARENTHESIS) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[PRIMARY] Expected '(' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

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

		if (read().type == TokenType::LITERAL_FLOATING_POINT || read().type == TokenType::LITERAL_INTEGER) {
			return parseNumberExpression();
		}

		if (read().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionCallOrGroupExpression();
		}

		compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[PRIMARY] Expected an expression but got " + getTokenString(read()) + ".", tokenIndex);

		return false;
	}

	bool Parser::parseIdentifierExpression() {
		ref<IdentifierExpression> expression = createRef<IdentifierExpression>();
		expression->identifierTokenIndex = tokenIndex;

		jump();

		ast.expressions.emplace_back(expression);

		if (read().type == TokenType::DOT) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseMemberAccessExpression() {
		jump();

		if (read().type < TokenType::USER_DEFINED) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[MEMBER_ACCESS] Expected an identifier but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

			if (read().type != TokenType::COMMA) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION_CALL] Expected ',' but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			jump();
		}

		if (read().type != TokenType::RIGHT_PARENTHESIS) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[FUNCTION_CALL] Expected ')' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

		if (read().type != TokenType::RIGHT_PARENTHESIS) {
			compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[GROUP] Expected ')' but got " + getTokenString(read()) + ".", tokenIndex);

			return false;
		}

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

			// Annotations are considered as user-defined identifiers because they can be used to name variables.
			if (read().type < TokenType::USER_DEFINED) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Expected an annotation but got " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			AnnotationInstance annotationInstance{};

			for (uint32 annotationIndex = 0; annotationIndex < ANNOTATION_COUNT; annotationIndex += 1) {
				const char8* annotation = ANNOTATION_STRINGS[annotationIndex];

				if (read().value == annotation) {
					annotationFlag |= BIT(annotationIndex);
					annotationInstance.annotation = static_cast<Annotation>(BIT(annotationIndex));

					break;
				}
			}

			if (annotationInstance.annotation == static_cast<Annotation>(0)) {
				compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Unknown annotation " + getTokenString(read()) + ".", tokenIndex);

				return false;
			}

			annotationInstance.identifier = read();

			jump();

			// The value can be provided either as a literal or as a constant identifier.
			if (read().type >= TokenType::LITERAL_FLOATING_POINT) {
				if (read().type >= TokenType::USER_DEFINED && !isScreamingSnakeCase(read().value.c_str())) {
					compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Constant identifier " + getTokenString(read()) + " is not written in screaming snake case.", tokenIndex);

					return false;
				}

				annotationInstance.value = read();

				compilationResult->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::CONSTANT);

				jump();
			}

			if (read().type != TokenType::RIGHT_SQUARE_BRACKET) {
				if (annotationInstance.value.value.empty()) {
					compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Expected a literal or ']' but got " + getTokenString(read()) + ".", tokenIndex);
				}
				else {
					compilationResult->registerDiagnostic(DiagnosticSeverity::ERROR, "[ANNOTATION_INSTANCE] Expected ']' but got " + getTokenString(read()) + ".", tokenIndex);
				}

				return false;
			}

			jump();

			ast.annotationInstances.emplace_back(annotationInstance);
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
}