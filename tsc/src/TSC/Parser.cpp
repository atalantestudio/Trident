#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
#include "Parser.hpp"

namespace ProjectA {
	#define EXPECT(condition, diagnosticCode, ...) \
		if (!(condition)) { \
			generateDiagnostic<diagnosticCode>(*output, __VA_ARGS__); \
			\
			BREAKPOINT(); \
			\
			return false; \
		}

	// TODO: Replace by EXPECT.
	#define EXPECT_OLD(condition, message) \
		if (!(condition)) { \
			output->generateDiagnostic(tokenIndex, message, DiagnosticSeverity::ERROR); \
			\
			return false; \
		}

	#define CONSUME(type) \
		EXPECT(read(type), DiagnosticCode::UNEXPECTED_TOKEN, type, read()) \
		\
		jump();

	#define CONSUME_IDENTIFIER() \
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read()) \
		\
		jump();

	#define PARSE_BINARY_EXPRESSION(condition, callback) \
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
		visitedWhileCount = 0;

		analyzer.setCompilationOutput(output);
		output->openScope(position);

		jumpEol();

		while (tokenIndex < output->tokens.size()) {
			if (!parseGlobalScopeStatement()) {
				synchronize();
			}

			jumpEol();
		}

		output->closeScope(position);
	}

	bool Parser::parseGlobalScopeStatement() {
		if (read(TokenType::IDENTIFIER) && peek().type == TokenType::LEFT_CURLY_BRACE) {
			return parseStructureStatement();
		}

		if (read(TokenType::LEFT_SQUARE_BRACKET) && !parseAnnotationBlocks()) {
			return false;
		}

		return parseConstantOrFunctionStatement();
	}

	bool Parser::parseStructureStatement() {
		ref<StructureStatement> statement = createRef<StructureStatement>();
		statement->identifierTokenIndex = tokenIndex;

		registerVariableSemanticToken(read(), true);

		const uint32 structureStatementIndex = static_cast<uint32>(output->ast.statements.size());

		jump();
		jump();
		jumpEol();

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());

		const uint32 statementIndex = getLastStatementIndex();

		while (!read(TokenType::RIGHT_CURLY_BRACE)) {
			if (read(TokenType::LEFT_SQUARE_BRACKET) && !parseAnnotationBlocks()) {
				return false;
			}

			if (!parseMemberStatement(statementIndex, statement)) {
				synchronize();

				break;
			}

			statement->memberCount += 1;

			EXPECT(
				read(TokenType::RIGHT_CURLY_BRACE) || read(TokenType::END_OF_LINE),
				DiagnosticCode::UNEXPECTED_TOKEN,
				"'}' or EOL",
				read()
			);

			jumpEol();
		}

		CONSUME(TokenType::RIGHT_CURLY_BRACE);

		analyzer.analyzeStructureStatement(statementIndex, statement, read().getEnd());

		return true;
	}

	bool Parser::parseMemberStatement(uint32 structureStatementIndex, ref<StructureStatement>& structureStatement) {
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
		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.statements.emplace_back(statement);
		analyzer.analyzeMemberStatement(structureStatementIndex, structureStatement, statement);

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
		statement->identifierTokenIndex = tokenIndex;

		registerVariableSemanticToken(read(), false);

		CONSUME_IDENTIFIER();

		EXPECT(read(TokenType::EQUAL), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::EQUAL, read());

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

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		jumpEol();

		return true;
	}

	// TODO: It should not be possible to annotate the return type of a function.
	bool Parser::parseFunctionStatement() {
		ref<FunctionStatement> statement = createRef<FunctionStatement>();
		statement->returnTypeExpressionReferenceIndex = getLastExpressionReferenceIndex();
		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		// We already know this is a '('.
		jump();

		jumpEol();

		while (!read(TokenType::RIGHT_PARENTHESIS)) {
			if (!parseParameterStatement()) {
				return false;
			}

			statement->parameterStatementIndices.emplace_back(getLastStatementIndex());

			if (read(TokenType::RIGHT_PARENTHESIS)) {
				break;
			}

			EXPECT_OLD(read(TokenType::COMMA) || read(TokenType::END_OF_LINE), "[FUNCTION] Expected ',' or EOL but got " + output->getTokenString(read()) + ".");

			jump();
			jumpEol();
		}

		CONSUME(TokenType::RIGHT_PARENTHESIS);
		CONSUME(TokenType::LEFT_CURLY_BRACE);

		statement->scopeStatementIndex = static_cast<uint32>(output->ast.statements.size());

		if (!parseScopeStatement()) {
			return false;
		}

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(getLastStatementIndex());
		analyzer.analyzeFunctionStatement(getLastStatementIndex(), position);

		return true;
	}

	bool Parser::parseParameterStatement() {
		if (!parseTypeExpression()) {
			return false;
		}

		ref<ParameterStatement> statement = createRef<ParameterStatement>();
		statement->typeExpressionReferenceIndex = getLastExpressionReferenceIndex();
		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.statements.emplace_back(statement);
		analyzer.analyzeParameterStatement(statement);

		return true;
	}

	bool Parser::parseLocalScopeStatement() {
		if (read(TokenType::LEFT_SQUARE_BRACKET)) {
			if (!parseAnnotationBlocks()) {
				return false;
			}
		}

		if (read(TokenType::LEFT_CURLY_BRACE)) {
			jump();

			return parseScopeStatement();
		}

		if (read(TokenType::KEYWORD_IF)) {
			return parseIfStatementStart();
		}

		if (read(TokenType::KEYWORD_RETURN)) {
			jump();

			return parseReturnStatement();
		}

		if (read(TokenType::KEYWORD_WHILE)) {
			return parseWhileStatementStart();
		}

		if (visitedWhileCount > 0) {
			if (read(TokenType::KEYWORD_BREAK)) {
				jump();

				return parseBreakStatement();
			}

			if (read(TokenType::KEYWORD_NEXT)) {
				jump();

				return parseNextStatement();
			}
		}

		if (read(TokenType::IDENTIFIER) && ((peek().type >= TokenType::EQUAL && peek().type <= TokenType::VERTICAL_BAR_EQUAL) || peek().type == TokenType::DOT)) {
			return parseExpressionStatement();
		}

		return parseVariableStatement();
	}

	bool Parser::parseBreakStatement() {
		output->ast.statements.emplace_back(createRef<BreakStatement>());

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		return true;
	}

	bool Parser::parseNextStatement() {
		output->ast.statements.emplace_back(createRef<NextStatement>());

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		return true;
	}

	bool Parser::parseExpressionStatement() {
		if (!parseAssignmentOrCompoundExpression()) {
			return false;
		}

		ref<ExpressionStatement> statement = createRef<ExpressionStatement>();
		statement->expressionReferenceIndex = getLastExpressionReferenceIndex();

		output->ast.statements.emplace_back(statement);

		// TODO: Why expect EOL?
		EXPECT(read(TokenType::RIGHT_CURLY_BRACE) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::RIGHT_CURLY_BRACE, read());

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
		output->openScope(position);

		jumpEol();

		while (!read(TokenType::RIGHT_CURLY_BRACE)) {
			if (!parseLocalScopeStatement()) {
				return false;
			}

			statement->statementIndices.emplace_back(getLastStatementIndex());

			jumpEol();
		}

		CONSUME(TokenType::RIGHT_CURLY_BRACE);

		output->closeScope(position);

		return true;
	}

	bool Parser::parseIfOrWhileStatementEnd(uint32& conditionExpressionReferenceIndex, uint32& scopeStatementIndex) {
		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		conditionExpressionReferenceIndex = getLastExpressionReferenceIndex();
		scopeStatementIndex = getLastStatementIndex() + 1;

		CONSUME(TokenType::LEFT_CURLY_BRACE);

		if (!parseScopeStatement()) {
			return false;
		}

		return true;
	}

	bool Parser::parseReturnStatement() {
		// NOTE: Parse logical OR or higher precedence expression.
		if (!parseLogicalOrExpression()) {
			return false;
		}

		ref<ReturnStatement> statement = createRef<ReturnStatement>();
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
		statement->identifierTokenIndex = tokenIndex;

		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

		registerVariableSemanticToken(read(), false);

		statement->constant = isScreamingSnakeCase(output->getIdentifier(read()));

		uint32 modifiers = 0;

		if (statement->constant) {
			modifiers |= SemanticTokenModifier::CONSTANT;
		}

		output->registerSemanticToken(tokenIndex, SemanticTokenType::VARIABLE, modifiers);

		jump();

		if (read(TokenType::EQUAL)) {
			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			statement->initializerExpressionReferenceIndex = getLastExpressionReferenceIndex();
		}
		else if (statement->constant) {
			generateDiagnostic<DiagnosticCode::UNEXPECTED_TOKEN>(*output, TokenType::EQUAL, read());
		}

		output->ast.statements.emplace_back(statement);
		analyzer.analyzeVariableStatement(statement);

		return true;
	}

	bool Parser::parseTypeExpression() {
		EXPECT_OLD(
			read(TokenType::IDENTIFIER) || (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX),
			"[TYPE] Expected a language or user-defined type but got " + output->getTokenString(read()) + "."
		);

		const TokenType type = read().type;
		uint32 typeIndex = 0;
		uint32 templateArgumentIndex = 0;

		if (type == TokenType::IDENTIFIER) {
			typeIndex = output->getUserDefinedTypeIndex(read().index);
		}

		Expression<ExpressionType::TYPE> expression{};
		expression.typeTokenIndex = tokenIndex;

		jump();

		if (read(TokenType::LEFT_ANGLE_BRACKET)) {
			jump();

			templateArgumentIndex = read().type;
			expression.templateArgumentTokenIndex = tokenIndex;

			EXPECT_OLD(
				templateArgumentIndex == TokenType::IDENTIFIER || (templateArgumentIndex >= FIRST_TYPE_INDEX && templateArgumentIndex <= LAST_TYPE_INDEX),
				"[TYPE] Expected a valid template argument but got " + output->getTokenString(read()) + "."
			);

			jump();

			CONSUME(TokenType::RIGHT_ANGLE_BRACKET);
		}

		expression.type = PackedType::fromTypeExpression(type, typeIndex, templateArgumentIndex);

		output->ast.addExpression(expression);
		analyzer.analyzeTypeExpression(expression);

		return true;
	}

	bool Parser::parseAssignmentOrCompoundExpression() {
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

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
		PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_OR), parseLogicalXorExpression);
	}

	bool Parser::parseLogicalXorExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_XOR), parseLogicalAndExpression);
	}

	bool Parser::parseLogicalAndExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_AND), parseBitwiseOrExpression);
	}

	bool Parser::parseBitwiseOrExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::VERTICAL_BAR), parseBitwiseXorExpression);
	}

	bool Parser::parseBitwiseXorExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::CARET), parseBitwiseAndExpression);
	}

	bool Parser::parseBitwiseAndExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::AMPERSAND), parseEqualityExpression);
	}

	bool Parser::parseEqualityExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::EQUAL_EQUAL) || read(TokenType::EXCLAMATION_EQUAL), parseRelationalExpression);
	}

	bool Parser::parseRelationalExpression() {
		PARSE_BINARY_EXPRESSION(read().type >= TokenType::LEFT_ANGLE_BRACKET && read().type <= TokenType::RIGHT_ANGLE_BRACKET_EQUAL, parseBitwiseShiftExpression);
	}

	bool Parser::parseBitwiseShiftExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET) || read(TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET), parseAdditiveExpression);
	}

	bool Parser::parseAdditiveExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::MINUS) || read(TokenType::PLUS), parseMultiplicativeExpression);
	}

	bool Parser::parseMultiplicativeExpression() {
		PARSE_BINARY_EXPRESSION(read(TokenType::ASTERISK) || read(TokenType::SLASH), parseUnaryExpression);
	}

	bool Parser::parseUnaryExpression() {
		if (read().type >= TokenType::PLUS && read().type <= TokenType::TILDE) {
			Expression<ExpressionType::UNARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(tokenIndex, SemanticTokenType::OPERATOR, 0);

			jump();

			// NOTE: Don't allow unary expressions following each other, e.g. --i.
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

			EXPECT(read(TokenType::LEFT_PARENTHESIS), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::LEFT_PARENTHESIS, read());

			return parseCallOrGroupExpression();
		}

		if (read(TokenType::IDENTIFIER) || (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX)) {
			if (!parseIdentifierExpression()) {
				return false;
			}

			if (read(TokenType::LEFT_PARENTHESIS)) {
				return parseCallOrGroupExpression();
			}

			return true;
		}

		if (read(TokenType::LITERAL_FLOATING_POINT) || read(TokenType::LITERAL_INTEGER)) {
			return parseLiteralExpression();
		}

		if (read(TokenType::LEFT_PARENTHESIS)) {
			return parseCallOrGroupExpression();
		}

		generateDiagnostic<DiagnosticCode::MISSING_EXPRESSION>(*output, read());

		return false;
	}

	bool Parser::parseIdentifierExpression() {
		Expression<ExpressionType::IDENTIFIER> expression{};
		expression.tokenIndex = tokenIndex;

		analyzer.analyzeIdentifierExpression(expression);
		output->ast.addExpression(expression);

		jump();

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseMemberAccessExpression() {
		jump();

		Expression<ExpressionType::MEMBER_ACCESS> expression{};
		expression.leftExpressionReferenceIndex = getLastExpressionReferenceIndex();
		expression.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.addExpression(expression);

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseCallOrGroupExpression() {
		const uint32 lastExpressionReferenceIndex = getLastExpressionReferenceIndex();
		const ExpressionReference& lastExpressionReference = output->ast.getExpressionReference(lastExpressionReferenceIndex);

		if (lastExpressionReference.type != ExpressionType::TYPE && lastExpressionReference.type != ExpressionType::IDENTIFIER) {
			return parseGroupExpression();
		}

		jump();

		Expression<ExpressionType::CALL> expression{};
		expression.calleeExpressionReferenceIndex = lastExpressionReferenceIndex;

		while (!read(TokenType::RIGHT_PARENTHESIS)) {
			// NOTE: Parse logical OR or higher precedence expression.
			if (!parseLogicalOrExpression()) {
				return false;
			}

			expression.argumentCount += 1;

			EXPECT(read(TokenType::RIGHT_PARENTHESIS) || read(TokenType::COMMA), DiagnosticCode::UNEXPECTED_TOKEN, "')' or ','", read());

			if (read(TokenType::COMMA)) {
				jump();
			}
		}

		CONSUME(TokenType::RIGHT_PARENTHESIS);

		output->ast.addExpression(expression);

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return true;
	}

	bool Parser::parseLiteralExpression() {
		Expression<ExpressionType::LITERAL> expression{};
		expression.tokenIndex = tokenIndex;

		output->ast.addExpression(expression);

		jump();

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

		output->ast.addExpression(expression);

		CONSUME(TokenType::RIGHT_PARENTHESIS);

		return true;
	}

	bool Parser::parseAnnotationBlocks() {
		while (read(TokenType::LEFT_SQUARE_BRACKET)) {
			jump();

			EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

			AnnotationBlock annotationBlock{};
			annotationBlock.identifierTokenIndex = tokenIndex;

			for (uint32 annotationIndex = 0; annotationIndex < ANNOTATION_COUNT; annotationIndex += 1) {
				const char8* annotation = ANNOTATION_STRINGS[annotationIndex];

				if (output->getTokenString(read()) == annotation) {
					annotationFlag |= BIT(annotationIndex);
					annotationBlock.annotation = static_cast<Annotation>(BIT(annotationIndex));

					break;
				}
			}

			if (annotationBlock.annotation == static_cast<Annotation>(0)) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_IDENTIFIER>(*output, read());
			}
			else {
				output->registerSemanticToken(annotationBlock.identifierTokenIndex, SemanticTokenType::VARIABLE, SemanticTokenModifier::PREDEFINED);
			}

			jump();

			if (!(ANNOTATION_TYPE_VOID & annotationBlock.annotation)) {
				if (!read(TokenType::LITERAL_FLOATING_POINT) && !read(TokenType::LITERAL_INTEGER)) {
					const Token& identifierToken = output->tokens[annotationBlock.identifierTokenIndex];

					output->generateDiagnostic(tokenIndex, "[ANNOTATION_INSTANCE] Expected a literal but got " + output->getTokenString(read()) + ".", DiagnosticSeverity::ERROR);
				}
				else {
					annotationBlock.valueTokenIndex = tokenIndex;

					jump();
				}
			}

			CONSUME(TokenType::RIGHT_SQUARE_BRACKET);

			jumpEol();

			output->ast.annotationBlocks.emplace_back(annotationBlock);
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

	bool Parser::read(TokenType type) const {
		return read().type == type;
	}

	Token Parser::read() const {
		if (tokenIndex >= output->tokens.size()) {
			return output->tokens.back();
		}

		return output->tokens[tokenIndex];
	}

	Token Parser::peek() const {
		if (tokenIndex + 1 >= output->tokens.size()) {
			return output->tokens.back();
		}

		return output->tokens[tokenIndex + 1];
	}

	void Parser::jump() {
		position = read().getEnd();
		tokenIndex += 1;
	}

	void Parser::jumpEol() {
		while (read(TokenType::END_OF_LINE) && tokenIndex < output->tokens.size()) {
			jump();
		}
	}

	void Parser::synchronize() {
		while (!read(TokenType::END_OF_LINE) && tokenIndex < output->tokens.size()) {
			jump();
		}

		jumpEol();
	}

	bool Parser::registerVariableSemanticToken(const Token& token, bool preferStructure) const {
		const std::string& identifier = output->getIdentifier(token);

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