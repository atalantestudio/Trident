#ifdef __EMSCRIPTEN__
	#include "Base/Base.hpp"
#else
	#include "pch.hpp"
#endif

#include "Diagnostic/Error.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

namespace ProjectA {
	// TODO: Move inside Base directory.
	bool Lexer::parseNumberLiteral(const char8* characters, uint32 characterCount, uint8 base, uint64& number) {
		uint32 characterIndex = 0;
		char8 character;
		uint8 digit;

		while (characterIndex < characterCount) {
			character = characters[characterIndex];
			digit = 0;

			if (character >= '0' && character <= '9') {
				// Parse 0-9 digit.
				digit = character - '0';
			}
			else if (character >= 'a' && character <= 'f') {
				// Parse 11-16 digit.
				digit = character - 'W';
			}

			#ifdef __EMSCRIPTEN__
				if (number > (std::numeric_limits<uint64>::max() - digit) / base) return false;
			#else
				if (number > (max<uint64>() - digit) / base) {
					// 64-bit number overflow.
					return false;
				}
			#endif

			number *= base;
			number += digit;

			characterIndex += 1;
		}

		return true;
	}

	void Lexer::tokenize(CompilationOutput* output) {
		this->output = output;
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

					output->registerSemanticToken(SemanticTokenType::COMMENT, start, position.characterOffset - start.characterOffset);

					break;
				}
				case '\r': {
					jump(1);
				}
				case '\n': {
					// TODO: Store the token range.
					registerEndOfToken(TokenType::END_OF_LINE);

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

		registerEndOfToken(TokenType::END_OF_LINE);
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
		token.type = TokenType::LITERAL_INTEGER;
		token.start = position;

		if (read() == '0') {
			const char8 prefix = peek(1);

			if (testLetter(prefix)) {
				jump(2);

				switch (prefix) {
					case 'b': {
						if (!testBinaryDigit(read())) {
							return;
						}

						while (testBinaryDigit(read())) {
							jump(1);
						}

						registerNumberLiteral(token, 2, 2);

						break;
					}
					case 'o': {
						if (!testOctalDigit(read())) {
							return;
						}

						while (testOctalDigit(read())) {
							jump(1);
						}

						registerNumberLiteral(token, 2, 8);

						break;
					}
					case 'h': {
						if (!testHexadecimalDigit(read())) {
							return;
						}

						while (testHexadecimalDigit(read())) {
							jump(1);
						}

						registerNumberLiteral(token, 2, 16);

						break;
					}
					default: {
						break;
					}
				}

				return;
			}

			// TODO: Invalid 0[0-9]{1} literal check.
		}

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

		registerNumberLiteral(token, 0, 10);
	}

	void Lexer::registerNumberLiteral(Token& token, uint32 offset, uint8 base) {
		token.characterCount = position.characterOffset - token.start.characterOffset;

		const char8* characters = &output->text[characterIndex - token.characterCount + offset];
		uint64 number = 0;

		if (!parseNumberLiteral(characters, token.characterCount - offset, base, number)) {
			generateDiagnostic<DiagnosticCode::NUMBER_OVERFLOW>(*output, token);
		}

		token.index = static_cast<uint32>(output->literals.size());

		output->literals.emplace_back(number);
		output->registerSemanticToken(token, SemanticTokenType::NUMBER);
		output->tokens.emplace_back(token);
	}

	void Lexer::readIdentifier(Token& token) {
		token.characterCount = position.characterOffset - token.start.characterOffset;

		const std::string identifier(&output->text[characterIndex - token.characterCount], token.characterCount);

		setTokenTypeAndIndex(token, identifier);

		output->tokens.emplace_back(token);

		if (token.type <= LAST_KEYWORD_INDEX) {
			output->registerSemanticToken(token, SemanticTokenType::KEYWORD);
		}
	}

	void Lexer::setTokenTypeAndIndex(Token& token, const std::string& identifier) const {
		uint32 identifierIndex = 0;

		// Test against reserved keywords.
		while (identifierIndex < RESERVED_KEYWORD_COUNT) {
			if (identifier == RESERVED_KEYWORDS[identifierIndex]) {
				token.type = static_cast<TokenType>(identifierIndex);

				return;
			}

			identifierIndex += 1;
		}

		token.type = TokenType::IDENTIFIER;

		identifierIndex = 0;

		// Test against user-defined identifiers.
		while (identifierIndex < output->identifiers.size()) {
			if (identifier == output->identifiers[identifierIndex]) {
				token.index = identifierIndex;

				return;
			}

			identifierIndex += 1;
		}

		token.index = static_cast<uint32>(identifierIndex);

		output->identifiers.emplace_back(identifier);
	}

	void Lexer::registerToken(TokenType type, uint32 characterCount) {
		output->registerToken(type, position, characterCount);

		jump(characterCount);
	}

	void Lexer::registerEndOfToken(TokenType type) {
		DocumentPosition start = position;

		if (output->tokens.size() > 0) {
			start = output->tokens.back().getEnd();
		}

		output->registerToken(type, start, 0);
	}
}