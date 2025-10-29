#pragma once

#include "DocumentPosition.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	struct Token {
		DocumentPosition getEnd() const {
			DocumentPosition end{};
			end.lineIndex = start.lineIndex;
			end.characterOffset = start.characterOffset + characterCount;

			return end;
		}

		public:
			TokenType type;
			DocumentPosition start;
			uint32 characterCount;

			// If `type` is TokenType::LITERAL_FLOATING_POINT or TokenType::LITERAL_INTEGER, `index` is the index of the referenced literal in CompilationOutput::literals.
			// If `type` is TokenType::IDENTIFIER, `index` is the index of the referenced identifier in CompilationOutput::identifiers.
			uint32 index = static_cast<uint32>(~0);
	};
}