#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Parser.hpp"

namespace ProjectA {
	#define TRY(callback) { \
		const DiagnosticCode code = callback; \
		\
		if (code != DiagnosticCode::SUCCESS) { \
			return code; \
		} \
	}

	#define EXPECT(condition, diagnosticCode, ...) if (!(condition)) { \
		return output->generateDiagnostic<diagnosticCode>(__VA_ARGS__); \
	}

	#define CONSUME(type) { \
		EXPECT(read(type), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(type), read()); \
		\
		jump(); \
	}

	#define CONSUME_IDENTIFIER() { \
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read()) \
		\
		jump(); \
	}

	#define PARSE_BINARY_EXPRESSION(condition, callback) { \
		const uint32 insertionIndex = output->ast.getExpressionCount(); \
		\
		TRY(callback()); \
		\
		while (condition) { \
			Expression<ExpressionType::BINARY> expression{}; \
			expression.operatorTokenIndex = tokenIndex; \
			\
			output->registerSemanticToken(SemanticTokenType::OPERATOR, expression.operatorTokenIndex); \
			\
			jump(); \
			\
			TRY(callback()); \
			\
			output->ast.insertExpression(expression, insertionIndex); \
		} \
		\
		return DiagnosticCode::SUCCESS; \
	}

	void Parser::parse(CompilationOutput* output) {
		this->output = output;
		output->ast = {};
		tokenIndex = 0;
		currentAnnotationCount = 0;
		annotationFlag = 0;
		position = {};
		visitedWhileCount = 0;

		jumpEol();

		while (tokenIndex < output->tokens.size()) {
			if (parseGlobalScopeStatement() != DiagnosticCode::SUCCESS) {
				synchronize();
			}

			jumpEol();
		}
	}

	DiagnosticCode Parser::parseGlobalScopeStatement() {
		if (read(TokenType::IDENTIFIER) && peek().type == TokenType::LEFT_CURLY_BRACE) {
			return parseStructureStatement();
		}

		if (read(TokenType::LEFT_SQUARE_BRACKET)) {
			TRY(parseAnnotationBlocks());
		}

		return parseConstantOrFunctionStatement();
	}

	DiagnosticCode Parser::parseStructureStatement() {
		Statement<StatementType::STRUCTURE> statement{};
		statement.identifierTokenIndex = tokenIndex;

		jump();
		jump();
		jumpEol();

		const uint32 statementByteOffset = output->ast.getStatementBufferByteSize();

		uint32 memberCount = 0;

		while (!read(TokenType::RIGHT_CURLY_BRACE) && !read(TokenType::END_OF_LINE)) {
			if (read(TokenType::LEFT_SQUARE_BRACKET)) {
				TRY(parseAnnotationBlocks());
			}

			if (parseMemberStatement() != DiagnosticCode::SUCCESS) {
				synchronize();

				break;
			}

			memberCount += 1;

			EXPECT(read(TokenType::RIGHT_CURLY_BRACE) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, "'}' or EOL", read());

			jumpEol();
		}

		CONSUME(TokenType::RIGHT_CURLY_BRACE);

		statement.memberCount = memberCount;
		statement.end = position;

		output->ast.insertStatement(statement, statementByteOffset);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseMemberStatement() {
		Statement<StatementType::MEMBER> statement{};
		statement.annotationBlockIndexOffset = currentAnnotationCount;
		statement.annotationBlockCount = static_cast<uint32>(output->ast.getAnnotationBlockCount()) - currentAnnotationCount;
		statement.annotationFlag = annotationFlag;

		currentAnnotationCount += statement.annotationBlockCount;
		annotationFlag = 0;

		TRY(parseTypeExpression());

		statement.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.addStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseConstantOrFunctionStatement() {
		Statement<StatementType::CONSTANT> statement{};
		statement.annotationBlockIndexOffset = currentAnnotationCount;
		statement.annotationBlockCount = static_cast<uint32>(output->ast.getAnnotationBlockCount()) - currentAnnotationCount;
		statement.annotationFlag = annotationFlag;

		currentAnnotationCount += statement.annotationBlockCount;
		annotationFlag = 0;

		TRY(parseTypeExpression());

		if (peek().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionStatement();
		}

		statement.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		EXPECT(read(TokenType::EQUAL), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(TokenType::EQUAL), read());

		output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

		jump();

		statement.initializerExpressionRange.start = read().start;

		TRY(parseLogicalOrExpression());

		statement.initializerExpressionRange.end = position;

		output->ast.addStatement(statement);

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(TokenType::END_OF_LINE), read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseFunctionStatement() {
		Statement<StatementType::FUNCTION> statement{};
		statement.returnType = true;
		statement.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		const uint32 statementByteOffset = output->ast.getStatementBufferByteSize();

		uint32 parameterCount = 0;

		// TODO: Position must be the end of the scope, but the scope statements must be analyzed in order.
		// analyzer.analyzeFunctionStatement(statement, output->ast.getExpressionCount(), position);

		// We already know it is a '('.
		jump();

		jumpEol();

		while (!read(TokenType::RIGHT_PARENTHESIS)) {
			TRY(parseParameterStatement());

			parameterCount += 1;

			if (read(TokenType::RIGHT_PARENTHESIS)) {
				break;
			}

			EXPECT(read(TokenType::COMMA) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, "',' or EOL", read());

			jump();
			jumpEol();
		}

		CONSUME(TokenType::RIGHT_PARENTHESIS);
		CONSUME(TokenType::LEFT_CURLY_BRACE);

		TRY(parseScopeStatement());

		statement.parameterCount = parameterCount;
		statement.end = position;

		output->ast.insertStatement(statement, statementByteOffset);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseParameterStatement() {
		TRY(parseTypeExpression());

		Statement<StatementType::PARAMETER> statement{};
		statement.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.addStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseLocalScopeStatement() {
		if (read(TokenType::LEFT_SQUARE_BRACKET)) {
			TRY(parseAnnotationBlocks());
		}

		if (read(TokenType::LEFT_CURLY_BRACE)) {
			jump();

			return parseScopeStatement();
		}

		if (read(TokenType::KEYWORD_IF)) {
			jump();

			return parseIfStatementStart();
		}

		if (read(TokenType::KEYWORD_RETURN)) {
			jump();

			return parseReturnStatement();
		}

		if (read(TokenType::KEYWORD_WHILE)) {
			jump();

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

	DiagnosticCode Parser::parseBreakStatement() {
		const Statement<StatementType::BREAK> statement{};

		output->ast.addStatement(statement);

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(TokenType::END_OF_LINE), read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseNextStatement() {
		const Statement<StatementType::NEXT> statement{};

		output->ast.addStatement(statement);

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(TokenType::END_OF_LINE), read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseExpressionStatement() {
		TRY(parseAssignmentOrCompoundExpression());

		const Statement<StatementType::EXPRESSION> statement{};

		output->ast.addStatement(statement);

		EXPECT(read(TokenType::RIGHT_CURLY_BRACE) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, "'}' or EOL", read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseIfStatementStart() {
		const Statement<StatementType::IF> statement{};

		output->ast.addStatement(statement);

		TRY(parseIfOrWhileStatementEnd());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseWhileStatementStart() {
		const Statement<StatementType::WHILE> statement{};

		output->ast.addStatement(statement);

		visitedWhileCount += 1;

		TRY(parseIfOrWhileStatementEnd());

		visitedWhileCount -= 1;

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseScopeStatement() {
		Statement<StatementType::SCOPE> statement{};
		statement.range.start = position;

		const uint32 statementByteOffset = output->ast.getStatementBufferByteSize();

		uint32 statementCount = 0;

		jumpEol();

		while (!read(TokenType::RIGHT_CURLY_BRACE)) {
			TRY(parseLocalScopeStatement());

			statementCount += 1;

			jumpEol();
		}

		CONSUME(TokenType::RIGHT_CURLY_BRACE);

		statement.statementCount = statementCount;
		statement.range.end = position;

		output->ast.insertStatement(statement, statementByteOffset);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseIfOrWhileStatementEnd() {
		TRY(parseLogicalOrExpression());

		CONSUME(TokenType::LEFT_CURLY_BRACE);

		TRY(parseScopeStatement());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseReturnStatement() {
		Statement<StatementType::RETURN> statement{};
		statement.expressionRange.start = read().start;

		TRY(parseLogicalOrExpression());

		statement.expressionRange.end = position;

		output->ast.addStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseVariableStatement() {
		TRY(parseTypeExpression());

		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

		Statement<StatementType::VARIABLE> statement{};
		statement.identifierTokenIndex = tokenIndex;
		statement.constant = isScreamingSnakeCase(output->getIdentifier(read()));
		statement.initializer = false;

		SemanticTokenType semanticTokenType = SemanticTokenType::VARIABLE;

		if (statement.constant) {
			statement.initializer = true;

			semanticTokenType = SemanticTokenType::CONSTANT;
		}

		// output->registerSemanticToken(semanticTokenType, tokenIndex);

		jump();

		if (read(TokenType::EQUAL)) {
			statement.initializer = true;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

			jump();

			statement.initializerExpressionRange.start = read().start;

			TRY(parseLogicalOrExpression());

			statement.initializerExpressionRange.end = position;
		}
		else if (statement.constant) {
			output->generateDiagnostic<DiagnosticCode::UNEXPECTED_TOKEN>(output->getNonIdentifierTokenString(TokenType::EQUAL), read());
		}

		output->ast.addStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseTypeExpression() {
		// TODO
		/* EXPECT_OLD(
			read(TokenType::IDENTIFIER) || (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX),
			"[TYPE] Expected a language or user-defined type but got " + output->getTokenString(read()) + "."
		); */

		Expression<ExpressionType::TYPE> expression{};
		expression.typeTokenIndex = tokenIndex;

		jump();

		if (read(TokenType::LEFT_ANGLE_BRACKET)) {
			jump();

			expression.templateArgumentTokenIndex = tokenIndex;

			// TODO
			/* EXPECT_OLD(
				templateArgumentIndex == TokenType::IDENTIFIER || (templateArgumentIndex >= FIRST_TYPE_INDEX && templateArgumentIndex <= LAST_TYPE_INDEX),
				"[TYPE] Expected a valid template argument but got " + output->getTokenString(read()) + "."
			); */

			jump();

			CONSUME(TokenType::RIGHT_ANGLE_BRACKET);
		}

		output->ast.addExpression(expression);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseAssignmentOrCompoundExpression() {
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

		const uint32 insertionIndex = output->ast.getExpressionCount();

		TRY(parseIdentifierExpression());

		if (read().type >= TokenType::EQUAL && read().type <= TokenType::VERTICAL_BAR_EQUAL) {
			Expression<ExpressionType::BINARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, expression.operatorTokenIndex);

			jump();

			TRY(parseLogicalOrExpression());

			output->ast.insertExpression(expression, insertionIndex);
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseLogicalOrExpression() PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_OR), parseLogicalXorExpression)
	DiagnosticCode Parser::parseLogicalXorExpression() PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_XOR), parseLogicalAndExpression)
	DiagnosticCode Parser::parseLogicalAndExpression() PARSE_BINARY_EXPRESSION(read(TokenType::KEYWORD_AND), parseBitwiseOrExpression)
	DiagnosticCode Parser::parseBitwiseOrExpression() PARSE_BINARY_EXPRESSION(read(TokenType::VERTICAL_BAR), parseBitwiseXorExpression)
	DiagnosticCode Parser::parseBitwiseXorExpression() PARSE_BINARY_EXPRESSION(read(TokenType::CARET), parseBitwiseAndExpression)
	DiagnosticCode Parser::parseBitwiseAndExpression() PARSE_BINARY_EXPRESSION(read(TokenType::AMPERSAND), parseEqualityExpression)
	DiagnosticCode Parser::parseEqualityExpression() PARSE_BINARY_EXPRESSION(read(TokenType::EQUAL_EQUAL) || read(TokenType::EXCLAMATION_EQUAL), parseRelationalExpression)
	DiagnosticCode Parser::parseRelationalExpression() PARSE_BINARY_EXPRESSION(read().type >= TokenType::LEFT_ANGLE_BRACKET && read().type <= TokenType::RIGHT_ANGLE_BRACKET_EQUAL, parseBitwiseShiftExpression)
	DiagnosticCode Parser::parseBitwiseShiftExpression() PARSE_BINARY_EXPRESSION(read(TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET) || read(TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET), parseAdditiveExpression)
	DiagnosticCode Parser::parseAdditiveExpression() PARSE_BINARY_EXPRESSION(read(TokenType::MINUS) || read(TokenType::PLUS), parseMultiplicativeExpression)
	DiagnosticCode Parser::parseMultiplicativeExpression() PARSE_BINARY_EXPRESSION(read(TokenType::ASTERISK) || read(TokenType::SLASH), parseUnaryExpression)

	DiagnosticCode Parser::parseUnaryExpression() {
		const uint32 insertionIndex = output->ast.getExpressionCount();

		// NOTE: Don't allow unary expressions following each other, e.g. --i.
		if (read().type >= TokenType::PLUS && read().type <= TokenType::TILDE) {
			Expression<ExpressionType::UNARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, expression.operatorTokenIndex);

			jump();

			TRY(parsePrimaryExpression());

			output->ast.insertExpression(expression, insertionIndex);

			return DiagnosticCode::SUCCESS;
		}

		return parsePrimaryExpression();
	}

	DiagnosticCode Parser::parsePrimaryExpression() {
		if (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX) {
			const uint32 insertionIndex = output->ast.getExpressionCount();

			TRY(parseTypeExpression());

			EXPECT(read(TokenType::LEFT_PARENTHESIS), DiagnosticCode::UNEXPECTED_TOKEN, output->getNonIdentifierTokenString(TokenType::LEFT_PARENTHESIS), read());

			return parseCallOrGroupExpression(insertionIndex, true);
		}

		if (read(TokenType::IDENTIFIER) || (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX)) {
			const uint32 insertionIndex = output->ast.getExpressionCount();

			TRY(parseIdentifierExpression());

			if (read(TokenType::LEFT_PARENTHESIS)) {
				return parseCallOrGroupExpression(insertionIndex, true);
			}

			return DiagnosticCode::SUCCESS;
		}

		if (read(TokenType::LITERAL_FLOATING_POINT) || read(TokenType::LITERAL_INTEGER)) {
			return parseLiteralExpression();
		}

		if (read(TokenType::LEFT_PARENTHESIS)) {
			const uint32 insertionIndex = output->ast.getExpressionCount();

			return parseCallOrGroupExpression(insertionIndex, false);
		}

		return output->generateDiagnostic<DiagnosticCode::MISSING_EXPRESSION>(read());
	}

	DiagnosticCode Parser::parseIdentifierExpression() {
		Expression<ExpressionType::IDENTIFIER> expression{};
		expression.tokenIndex = tokenIndex;

		output->ast.addExpression(expression);

		jump();

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseMemberAccessExpression() {
		jump();

		Expression<ExpressionType::MEMBER_ACCESS> expression{};
		expression.identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.addExpression(expression);

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseCallOrGroupExpression(uint32 insertionIndex, bool explicitCallee) {
		// TODO: Refactor.
		const ExpressionReference& lastExpressionReference = output->ast.getExpressionReference(output->ast.getExpressionCount() - 1);

		if (lastExpressionReference.type != ExpressionType::TYPE && lastExpressionReference.type != ExpressionType::IDENTIFIER) {
			return parseGroupExpression();
		}

		jump();

		Expression<ExpressionType::CALL> expression{};
		expression.explicitCallee = explicitCallee;

		while (!read(TokenType::RIGHT_PARENTHESIS)) {
			TRY(parseLogicalOrExpression());

			expression.argumentCount += 1;

			EXPECT(read(TokenType::RIGHT_PARENTHESIS) || read(TokenType::COMMA), DiagnosticCode::UNEXPECTED_TOKEN, "')' or ','", read());

			if (read(TokenType::COMMA)) {
				jump();
			}
		}

		CONSUME(TokenType::RIGHT_PARENTHESIS);

		output->ast.insertExpression(expression, insertionIndex);

		if (read(TokenType::DOT)) {
			return parseMemberAccessExpression();
		}

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseLiteralExpression() {
		Expression<ExpressionType::LITERAL> expression{};
		expression.tokenIndex = tokenIndex;

		output->ast.addExpression(expression);

		jump();

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseGroupExpression() {
		jump();

		TRY(parseLogicalOrExpression());

		Expression<ExpressionType::GROUP> expression{};

		output->ast.addExpression(expression);

		CONSUME(TokenType::RIGHT_PARENTHESIS);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseAnnotationBlocks() {
		while (read(TokenType::LEFT_SQUARE_BRACKET)) {
			jump();

			EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

			AnnotationBlock annotationBlock{};
			annotationBlock.identifierTokenIndex = tokenIndex;

			uint32 annotation = 0;

			// Find which annotation this block refers to.
			while (annotation < ANNOTATION_COUNT) {
				if (output->getTokenString(read()) == ANNOTATION_STRINGS[annotation]) {
					annotationBlock.annotation = static_cast<Annotation>(annotation);
					annotationFlag |= BIT(annotation);

					// output->registerSemanticToken(SemanticTokenType::PREDEFINED, annotationBlock.identifierTokenIndex);

					break;
				}

				annotation += 1;
			}

			if (annotationBlock.annotation == static_cast<Annotation>(~0)) {
				output->generateDiagnostic<DiagnosticCode::UNDEFINED_ANNOTATION>(read(), output->getTokenString(read()));
			}

			jump();

			if (annotationBlock.annotation != static_cast<Annotation>(~0) && !(ANNOTATION_TYPE_NONE & BIT(annotationBlock.annotation))) {
				if (read(TokenType::LITERAL_INTEGER)) {
					annotationBlock.valueTokenIndex = tokenIndex;

					jump();
				}
				else {
					output->generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(read(), output->getTokenString(read()));

					// TODO: Synchronize?
				}
			}

			CONSUME(TokenType::RIGHT_SQUARE_BRACKET);

			// Allow a single EOL between annotations.
			if (read(TokenType::END_OF_LINE)) {
				jump();
			}

			output->ast.addAnnotationBlock(annotationBlock);
		}

		return DiagnosticCode::SUCCESS;
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

	bool Parser::read(TokenType type) const {
		return read().type == type;
	}

	void Parser::jump() {
		position = read().getRange().end;
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
}