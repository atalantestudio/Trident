#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Case.hpp"
#include "Diagnostic/Error.hpp"
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
		generateDiagnostic<diagnosticCode>(*output, __VA_ARGS__); \
		\
		BREAKPOINT(); \
		\
		return diagnosticCode; \
	}

	#define CONSUME(type) { \
		EXPECT(read(type), DiagnosticCode::UNEXPECTED_TOKEN, type, read()) \
		\
		jump(); \
	}

	#define CONSUME_IDENTIFIER() { \
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read()) \
		\
		jump(); \
	}

	#define PARSE_BINARY_EXPRESSION(condition, callback) { \
		const uint32 insertionIndex = output->ast.getExpressionReferenceCount(); \
		\
		TRY(callback()); \
		\
		while (condition) { \
			Expression<ExpressionType::BINARY> expression{}; \
			expression.operatorTokenIndex = tokenIndex; \
			\
			output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex); \
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

		analyzer.setCompilationOutput(output);
		output->openScope(position);

		jumpEol();

		while (tokenIndex < output->tokens.size()) {
			if (parseGlobalScopeStatement() != DiagnosticCode::SUCCESS) {
				synchronize();
			}

			jumpEol();
		}

		output->closeScope(position);
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
		ref<StructureStatement> statement = createRef<StructureStatement>();
		statement->identifierTokenIndex = tokenIndex;

		const uint32 structureStatementIndex = static_cast<uint32>(output->ast.statements.size());

		jump();
		jump();
		jumpEol();

		output->ast.statements.emplace_back(statement);

		const uint32 statementIndex = static_cast<uint32>(output->ast.statements.size() - 1);

		output->ast.globalScopeStatementIndices.emplace_back(statementIndex);

		const uint32 memberTypeExpressionReferenceOffset = output->ast.getExpressionReferenceCount();

		while (!read(TokenType::RIGHT_CURLY_BRACE) && !read(TokenType::END_OF_LINE)) {
			if (read(TokenType::LEFT_SQUARE_BRACKET)) {
				TRY(parseAnnotationBlocks());
			}

			if (parseMemberStatement(statementIndex, statement) != DiagnosticCode::SUCCESS) {
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

		analyzer.analyzeStructureStatement(statementIndex, statement, read().getEnd(), memberTypeExpressionReferenceOffset);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseMemberStatement(uint32 structureStatementIndex, ref<StructureStatement>& structureStatement) {
		ref<MemberStatement> statement = createRef<MemberStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationBlockIndexOffset = currentAnnotationCount;
		statement->annotationBlockCount = static_cast<uint32>(output->ast.annotationBlocks.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationBlockCount;
		annotationFlag = 0;

		TRY(parseTypeExpression());

		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.statements.emplace_back(statement);

		analyzer.analyzeMemberStatement(structureStatementIndex, structureStatement, statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseConstantOrFunctionStatement() {
		ref<ConstantStatement> statement = createRef<ConstantStatement>();
		statement->annotationFlag = annotationFlag;
		statement->annotationBlockIndexOffset = currentAnnotationCount;
		statement->annotationBlockCount = static_cast<uint32>(output->ast.annotationBlocks.size()) - currentAnnotationCount;

		currentAnnotationCount += statement->annotationBlockCount;
		annotationFlag = 0;

		TRY(parseTypeExpression());

		if (peek().type == TokenType::LEFT_PARENTHESIS) {
			return parseFunctionStatement();
		}

		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		EXPECT(read(TokenType::EQUAL), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::EQUAL, read());

		output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

		jump();

		TRY(parseLogicalOrExpression());

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(static_cast<uint32>(output->ast.statements.size() - 1));

		analyzer.analyzeConstantStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	// TODO: It should not be possible to annotate the return type of a function.
	DiagnosticCode Parser::parseFunctionStatement() {
		ref<FunctionStatement> statement = createRef<FunctionStatement>();
		statement->returnType = true;
		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.statements.emplace_back(statement);
		output->ast.globalScopeStatementIndices.emplace_back(static_cast<uint32>(output->ast.statements.size() - 1));

		const uint32 parameterTypeExpressionOffset = output->ast.getExpressionReferenceCount();

		// TODO: Position must be the end of the scope, but the scope statements must be analyzed in order.
		analyzer.analyzeFunctionStatement(statement, parameterTypeExpressionOffset, position);

		// We already know it is a '('.
		jump();

		jumpEol();

		while (!read(TokenType::RIGHT_PARENTHESIS)) {
			TRY(parseParameterStatement());

			statement->parameterStatementIndices.emplace_back(static_cast<uint32>(output->ast.statements.size() - 1));

			if (read(TokenType::RIGHT_PARENTHESIS)) {
				break;
			}

			EXPECT(read(TokenType::COMMA) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, "',' or EOL", read());

			jump();
			jumpEol();
		}

		CONSUME(TokenType::RIGHT_PARENTHESIS);
		CONSUME(TokenType::LEFT_CURLY_BRACE);

		statement->scopeStatementIndex = static_cast<uint32>(output->ast.statements.size());

		TRY(parseScopeStatement());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseParameterStatement() {
		TRY(parseTypeExpression());

		ref<ParameterStatement> statement = createRef<ParameterStatement>();
		statement->identifierTokenIndex = tokenIndex;

		CONSUME_IDENTIFIER();

		output->ast.statements.emplace_back(statement);
		// analyzer.analyzeParameterStatement(statement);

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
		output->ast.statements.emplace_back(createRef<BreakStatement>());

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseNextStatement() {
		output->ast.statements.emplace_back(createRef<NextStatement>());

		EXPECT(read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::END_OF_LINE, read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseExpressionStatement() {
		TRY(parseAssignmentOrCompoundExpression());

		const ref<ExpressionStatement> statement = createRef<ExpressionStatement>();

		output->ast.statements.emplace_back(statement);

		// TODO: Why expect EOL?
		// EXPECT(read(TokenType::RIGHT_CURLY_BRACE) || read(TokenType::END_OF_LINE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::RIGHT_CURLY_BRACE, read());
		EXPECT(read(TokenType::RIGHT_CURLY_BRACE), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::RIGHT_CURLY_BRACE, read());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseIfStatementStart() {
		ref<IfStatement> statement = createRef<IfStatement>();

		TRY(parseIfOrWhileStatementEnd(statement->scopeStatementIndex));

		output->ast.statements.emplace_back(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseWhileStatementStart() {
		visitedWhileCount += 1;

		ref<WhileStatement> statement = createRef<WhileStatement>();

		TRY(parseIfOrWhileStatementEnd(statement->scopeStatementIndex));

		output->ast.statements.emplace_back(statement);

		visitedWhileCount -= 1;

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseScopeStatement() {
		ref<ScopeStatement> statement = createRef<ScopeStatement>();

		output->ast.statements.emplace_back(statement);
		output->openScope(position);

		jumpEol();

		while (!read(TokenType::RIGHT_CURLY_BRACE)) {
			TRY(parseLocalScopeStatement());

			statement->statementIndices.emplace_back(static_cast<uint32>(output->ast.statements.size()) - 1);

			jumpEol();
		}

		CONSUME(TokenType::RIGHT_CURLY_BRACE);

		output->closeScope(position);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseIfOrWhileStatementEnd(uint32& scopeStatementIndex) {
		TRY(parseLogicalOrExpression());

		scopeStatementIndex = static_cast<uint32>(output->ast.statements.size());

		CONSUME(TokenType::LEFT_CURLY_BRACE);

		TRY(parseScopeStatement());

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseReturnStatement() {
		TRY(parseLogicalOrExpression());

		const ref<ReturnStatement> statement = createRef<ReturnStatement>();

		output->ast.statements.emplace_back(statement);

		analyzer.analyzeReturnStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseVariableStatement() {
		TRY(parseTypeExpression());

		ref<VariableStatement> statement = createRef<VariableStatement>();
		statement->identifierTokenIndex = tokenIndex;

		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

		statement->constant = isScreamingSnakeCase(output->getIdentifier(read()));
		statement->initializer = false;

		SemanticTokenType semanticTokenType = SemanticTokenType::VARIABLE;

		if (statement->constant) {
			statement->initializer = true;

			semanticTokenType = SemanticTokenType::CONSTANT;
		}

		output->registerSemanticToken(semanticTokenType, tokenIndex);

		jump();

		if (read(TokenType::EQUAL)) {
			statement->initializer = true;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

			jump();

			TRY(parseLogicalOrExpression());
		}
		else if (statement->constant) {
			generateDiagnostic<DiagnosticCode::UNEXPECTED_TOKEN>(*output, TokenType::EQUAL, read());
		}

		output->ast.statements.emplace_back(statement);

		analyzer.analyzeVariableStatement(statement);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseTypeExpression() {
		// TODO
		/* EXPECT_OLD(
			read(TokenType::IDENTIFIER) || (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX),
			"[TYPE] Expected a language or user-defined type but got " + output->getTokenString(read()) + "."
		); */

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

			// TODO
			/* EXPECT_OLD(
				templateArgumentIndex == TokenType::IDENTIFIER || (templateArgumentIndex >= FIRST_TYPE_INDEX && templateArgumentIndex <= LAST_TYPE_INDEX),
				"[TYPE] Expected a valid template argument but got " + output->getTokenString(read()) + "."
			); */

			jump();

			CONSUME(TokenType::RIGHT_ANGLE_BRACKET);
		}

		expression.type = PackedType::fromTypeExpression(type, typeIndex, templateArgumentIndex);

		output->ast.addExpression(expression);

		return DiagnosticCode::SUCCESS;
	}

	DiagnosticCode Parser::parseAssignmentOrCompoundExpression() {
		EXPECT(read(TokenType::IDENTIFIER), DiagnosticCode::MISSING_IDENTIFIER, read());

		const uint32 insertionIndex = output->ast.getExpressionReferenceCount();

		TRY(parseIdentifierExpression());

		if (read().type >= TokenType::EQUAL && read().type <= TokenType::VERTICAL_BAR_EQUAL) {
			Expression<ExpressionType::BINARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

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
		const uint32 insertionIndex = output->ast.getExpressionReferenceCount();

		if (read().type >= TokenType::PLUS && read().type <= TokenType::TILDE) {
			Expression<ExpressionType::UNARY> expression{};
			expression.operatorTokenIndex = tokenIndex;

			output->registerSemanticToken(SemanticTokenType::OPERATOR, tokenIndex);

			jump();

			// NOTE: Don't allow unary expressions following each other, e.g. --i.
			TRY(parsePrimaryExpression());

			output->ast.insertExpression(expression, insertionIndex);

			return DiagnosticCode::SUCCESS;
		}

		return parsePrimaryExpression();
	}

	DiagnosticCode Parser::parsePrimaryExpression() {
		if (read().type >= FIRST_TYPE_INDEX && read().type <= LAST_TYPE_INDEX) {
			const uint32 insertionIndex = output->ast.getExpressionReferenceCount();

			TRY(parseTypeExpression());

			EXPECT(read(TokenType::LEFT_PARENTHESIS), DiagnosticCode::UNEXPECTED_TOKEN, TokenType::LEFT_PARENTHESIS, read());

			return parseCallOrGroupExpression(insertionIndex, true);
		}

		if (read(TokenType::IDENTIFIER) || (read().type >= FIRST_FUNCTION_INDEX && read().type <= LAST_FUNCTION_INDEX)) {
			const uint32 insertionIndex = output->ast.getExpressionReferenceCount();

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
			const uint32 insertionIndex = output->ast.getExpressionReferenceCount();

			return parseCallOrGroupExpression(insertionIndex, false);
		}

		generateDiagnostic<DiagnosticCode::MISSING_EXPRESSION>(*output, read());

		return DiagnosticCode::MISSING_EXPRESSION;
	}

	DiagnosticCode Parser::parseIdentifierExpression() {
		Expression<ExpressionType::IDENTIFIER> expression{};
		expression.tokenIndex = tokenIndex;

		// analyzer.analyzeIdentifierExpression(expression);
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
		const ExpressionReference& lastExpressionReference = output->ast.getExpressionReference(static_cast<uint32>(output->ast.expressionReferences.size()) - 1);

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

			// Find which annotation this block refers to.
			for (uint32 annotation = 0; annotation < ANNOTATION_COUNT; annotation += 1) {
				if (output->getTokenString(read()) == ANNOTATION_STRINGS[annotation]) {
					annotationBlock.annotation = static_cast<Annotation>(annotation);
					annotationFlag |= BIT(annotation);

					output->registerSemanticToken(SemanticTokenType::PREDEFINED, annotationBlock.identifierTokenIndex);

					break;
				}
			}

			if (annotationBlock.annotation == UNDEFINED_ANNOTATION) {
				generateDiagnostic<DiagnosticCode::UNDEFINED_ANNOTATION>(*output, read());
			}

			jump();

			if (annotationBlock.annotation != UNDEFINED_ANNOTATION && !(ANNOTATION_TYPE_NONE & BIT(annotationBlock.annotation))) {
				if (read(TokenType::LITERAL_INTEGER)) {
					annotationBlock.valueTokenIndex = tokenIndex;

					jump();
				}
				else {
					generateDiagnostic<DiagnosticCode::MISSING_ANNOTATION_VALUE>(*output, read());

					// TODO: Synchronize?
				}
			}

			CONSUME(TokenType::RIGHT_SQUARE_BRACKET);

			// Allow a single EOL between annotations.
			if (read(TokenType::END_OF_LINE)) {
				jump();
			}

			output->ast.annotationBlocks.emplace_back(annotationBlock);
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
}