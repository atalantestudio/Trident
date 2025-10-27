#pragma once

#include "CompilationOutput.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	class Lexer {
		private:
			static bool parseNumberLiteral(const char8* characters, uint32 characterCount, uint8 base, uint64& number);

		public:
			void tokenize(CompilationOutput* _output);

		private:
			void jump(sint32 characterCount);

			char8 read() const;
			char8 peek(uint32 index) const;

			bool testBinaryDigit(char8 character) const;
			bool testOctalDigit(char8 character) const;
			bool testDecimalDigit(char8 character) const;
			bool testHexadecimalDigit(char8 character) const;
			bool testLetter(char8 character) const;
			bool testAlphanumeric(char8 character) const;

			void readNumberLiteral();
			void registerNumberLiteral(Token& token, uint32 offset, uint8 base);
			void readIdentifier(Token& token);

			uint32 getIdentifierTokenTypeIndex(const std::string& identifier) const;

			void registerToken(TokenType type, uint32 characterCount);
			void registerEndOfToken(TokenType type);

		private:
			CompilationOutput* output;
			DocumentPosition position;
			uint32 characterIndex;
			bool unexpectedCharacter;
	};
}