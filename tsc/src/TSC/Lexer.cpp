#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Diagnostic/Error.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

namespace ProjectA {
	void Lexer::tokenize(CompilationOutput* _output) {
		output = _output;

		position.lineIndex = 0;
		position.characterOffset = 0;
		characterIndex = 0;
		unexpectedCharacter = false;

		while (characterIndex < output->text.size()) {
			if (testLetter(read())) {
				Token token{};
				token.start = position;
				token.characterCount = 0;

				jump(1);

				while (testAlphanumeric(read())) {
					jump(1);
				}

				readIdentifier(token);

				continue;
			}

			if (testDecimalDigit(read())) {
				readNumberLiteral();

				continue;
			}

			const char8 character = read();

			switch (character) {
				case ',': {
					registerToken(TokenType::COMMA, 1);

					break;
				}
				case '.': {
					registerToken(TokenType::DOT, 1);

					break;
				}
				case '{': {
					registerToken(TokenType::LEFT_CURLY_BRACE, 1);

					break;
				}
				case '(': {
					registerToken(TokenType::LEFT_PARENTHESIS, 1);

					break;
				}
				case '[': {
					registerToken(TokenType::LEFT_SQUARE_BRACKET, 1);

					break;
				}
				case '}': {
					registerToken(TokenType::RIGHT_CURLY_BRACE, 1);

					break;
				}
				case ')': {
					registerToken(TokenType::RIGHT_PARENTHESIS, 1);

					break;
				}
				case ']': {
					registerToken(TokenType::RIGHT_SQUARE_BRACKET, 1);

					break;
				}
				case '~': {
					registerToken(TokenType::TILDE, 1);

					break;
				}
				case '&': {
					if (peek(1) == '=') {
						registerToken(TokenType::AMPERSAND_EQUAL, 2);
					}
					else {
						registerToken(TokenType::AMPERSAND, 1);
					}

					break;
				}
				case '*': {
					if (peek(1) == '=') {
						registerToken(TokenType::ASTERISK_EQUAL, 2);
					}
					else {
						registerToken(TokenType::ASTERISK, 1);
					}

					break;
				}
				case '^': {
					if (peek(1) == '=') {
						registerToken(TokenType::CARET_EQUAL, 2);
					}
					else {
						registerToken(TokenType::CARET, 1);
					}

					break;
				}
				case '+': {
					if (peek(1) == '=') {
						registerToken(TokenType::PLUS_EQUAL, 2);
					}
					else {
						registerToken(TokenType::PLUS, 1);
					}

					break;
				}
				case '-': {
					if (peek(1) == '=') {
						registerToken(TokenType::MINUS_EQUAL, 2);
					}
					else {
						registerToken(TokenType::MINUS, 1);
					}

					break;
				}
				case '|': {
					if (peek(1) == '=') {
						registerToken(TokenType::VERTICAL_BAR_EQUAL, 2);
					}
					else {
						registerToken(TokenType::VERTICAL_BAR, 1);
					}

					break;
				}
				case '=': {
					if (peek(1) == '=') {
						registerToken(TokenType::EQUAL_EQUAL, 2);
					}
					else {
						registerToken(TokenType::EQUAL, 1);
					}

					break;
				}
				case '!': {
					if (peek(1) == '=') {
						registerToken(TokenType::EXCLAMATION_EQUAL, 2);
					}
					else {
						registerToken(TokenType::EXCLAMATION, 1);
					}

					break;
				}
				case '<': {
					switch (peek(1)) {
						case '<': {
							if (peek(2) == '=') {
								registerToken(TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL, 3);
							}
							else {
								registerToken(TokenType::LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET, 2);
							}

							break;
						}
						case '=': {
							registerToken(TokenType::LEFT_ANGLE_BRACKET_EQUAL, 2);

							break;
						}
						default: {
							registerToken(TokenType::LEFT_ANGLE_BRACKET, 1);

							break;
						}
					}

					break;
				}
				case '>': {
					switch (peek(1)) {
						case '>': {
							if (peek(2) == '=') {
								registerToken(TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL, 3);
							}
							else {
								registerToken(TokenType::RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET, 2);
							}

							break;
						}
						case '=': {
							registerToken(TokenType::RIGHT_ANGLE_BRACKET_EQUAL, 2);

							break;
						}
						default: {
							registerToken(TokenType::RIGHT_ANGLE_BRACKET, 1);

							break;
						}
					}

					break;
				}
				case '/': {
					if (peek(1) == '=') {
						registerToken(TokenType::SLASH_EQUAL, 2);

						break;
					}

					if (peek(1) != '/') {
						registerToken(TokenType::SLASH, 1);

						break;
					}

					const DocumentPosition start = position;

					jump(2);

					while (read() != '\n' && read() != '\0') {
						jump(1);
					}

					output->registerSemanticToken(SemanticTokenType::COMMENT, 0, start, position.characterOffset - start.characterOffset);
				}
				case '\r': {
					jump(1);
				}
				case '\n': {
					output->registerToken(TokenType::END_OF_LINE, position, 0);

					while (read() == '\n') {
						characterIndex += 1;
						position.lineIndex += 1;
						position.characterOffset = 0;

						if (read() == '\r') {
							jump(1);
						}
					}

					break;
				}
				default: {
					if (!unexpectedCharacter) {
						generateDiagnostic<DiagnosticCode::UNEXPECTED_CHARACTER>(*output, character, position);

						unexpectedCharacter = true;
					}
				}
				case '\t':
				case ' ': {
					jump(1);

					break;
				}
			}
		}

		registerToken(TokenType::END_OF_FILE, 0);
	}

	void Lexer::jump(sint32 characterCount) {
		if (static_cast<sint32>(characterIndex) + characterCount < 0) {
			return;
		}

		characterIndex += characterCount;
		position.characterOffset += characterCount;
	}

	char8 Lexer::read() const {
		return peek(0);
	}

	char8 Lexer::peek(uint32 index) const {
		if (characterIndex + index >= output->text.size()) {
			return '\0';
		}

		return output->text[characterIndex + index];
	}

	bool Lexer::testBinaryDigit(char8 character) const {
		return character == '0' || character == '1';
	}

	bool Lexer::testOctalDigit(char8 character) const {
		return character >= '0' && character <= '7';
	}

	bool Lexer::testDecimalDigit(char8 character) const {
		return character >= '0' && character <= '9';
	}

	bool Lexer::testHexadecimalDigit(char8 character) const {
		return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f');
	}

	bool Lexer::testLetter(char8 character) const {
		return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || character == '_';
	}

	bool Lexer::testAlphanumeric(char8 character) const {
		return testLetter(character) || testDecimalDigit(character);
	}

	void Lexer::readNumberLiteral() {
		Token token{};
		token.start = position;

		if (read() == '0' && testLetter(peek(1))) {
			const char8 prefix = peek(1);

			jump(2);

			switch (prefix) {
				case 'b': {
					if (!testBinaryDigit(read())) {
						return;
					}

					while (testBinaryDigit(read())) {
						jump(1);
					}

					token.type = TokenType::LITERAL_INTEGER_BASE_2;

					registerNumberLiteral(token);

					break;
				}
				case 'o': {
					if (!testOctalDigit(read())) {
						return;
					}

					while (testOctalDigit(read())) {
						jump(1);
					}

					token.type = TokenType::LITERAL_INTEGER_BASE_8;

					registerNumberLiteral(token);

					break;
				}
				case 'h': {
					if (!testHexadecimalDigit(read())) {
						return;
					}

					while (testHexadecimalDigit(read())) {
						jump(1);
					}

					token.type = TokenType::LITERAL_INTEGER_BASE_16;

					registerNumberLiteral(token);

					break;
				}
				default: {
					break;
				}
			}

			return;
		}

		token.type = TokenType::LITERAL_INTEGER_BASE_10;

		jump(1);

		while (testDecimalDigit(read())) {
			jump(1);
		}

		if (read() == '.' && testDecimalDigit(peek(1))) {
			token.type = TokenType::LITERAL_FLOATING_POINT;

			jump(2);

			while (testDecimalDigit(read())) {
				jump(1);
			}
		}

		registerNumberLiteral(token);
	}

	void Lexer::registerNumberLiteral(Token& token) {
		token.characterCount = position.characterOffset - token.start.characterOffset;
		token.value = &output->text[characterIndex - token.characterCount];

		output->registerSemanticToken(SemanticTokenType::NUMBER, 0, token.start, token.characterCount);
		output->tokens.emplace_back(token);
	}

	void Lexer::readIdentifier(Token& token) {
		token.characterCount = position.characterOffset - token.start.characterOffset;

		const std::string identifier(&output->text[characterIndex - token.characterCount], token.characterCount);

		token.type = static_cast<TokenType>(getIdentifierTokenTypeIndex(identifier));

		output->tokens.emplace_back(token);

		if (token.type <= LAST_KEYWORD_INDEX) {
			output->registerSemanticToken(token, SemanticTokenType::KEYWORD, 0);
		}
	}

	uint32 Lexer::getIdentifierTokenTypeIndex(const std::string& identifier) const {
		uint32 identifierIndex = 0;

		// Test against reserved keywords.
		while (identifierIndex < RESERVED_KEYWORD_COUNT) {
			if (identifier == RESERVED_KEYWORDS[identifierIndex]) {
				return identifierIndex;
			}

			identifierIndex += 1;
		}

		identifierIndex = 0;

		// Test against user-defined identifiers.
		while (identifierIndex < output->userDefinedIdentifiers.size()) {
			if (identifier == output->userDefinedIdentifiers[identifierIndex]) {
				return TokenType::USER_DEFINED + identifierIndex;
			}

			identifierIndex += 1;
		}

		output->userDefinedIdentifiers.emplace_back(identifier);

		return TokenType::USER_DEFINED + identifierIndex;
	}

	void Lexer::registerToken(TokenType type, uint32 characterCount) {
		output->registerToken(type, position, characterCount);

		jump(characterCount);
	}
}