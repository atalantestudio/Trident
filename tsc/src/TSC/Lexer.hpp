#pragma once

#include "CompilationOutput.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	class Lexer {
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
			void registerNumberLiteral(Token& token);
			void readIdentifier(Token& token);

			uint32 getIdentifierTokenTypeIndex(const std::string& identifier) const;

			void registerToken(TokenType type, uint32 characterCount);

		private:
			CompilationOutput* output;
			DocumentPosition position;
			uint32 characterIndex;
			bool unexpectedCharacter;
	};
}