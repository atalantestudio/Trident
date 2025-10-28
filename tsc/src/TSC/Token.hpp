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

			// If `type` is TokenType::LITERAL_FLOATING_POINT or TokenType::LITERAL_INTEGER, use to index into CompilationOutput::literals.
			// If `type` is TokenType::USER_DEFINED and the token is used to declare a structure, use to index into CompilationOutput::userDefinedTypes.
			uint32 index = static_cast<uint32>(~0);
	};

	inline std::string getOperatorTokenString(const Token& token) {
		switch (token.type) {
			case EQUAL: return "=";
			case ASTERISK_EQUAL: return "*=";
			case SLASH_EQUAL: return "/=";
			case PLUS_EQUAL: return "+=";
			case MINUS_EQUAL: return "-=";
			case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL: return "<<=";
			case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL: return ">>=";
			case AMPERSAND_EQUAL: return "&=";
			case CARET_EQUAL: return "^=";
			case VERTICAL_BAR_EQUAL: return "|=";
			case VERTICAL_BAR: return "|";
			case CARET: return "^";
			case AMPERSAND: return "&";
			case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: return "<<";
			case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: return ">>";
			case EQUAL_EQUAL: return "==";
			case EXCLAMATION_EQUAL: return "!=";
			case LEFT_ANGLE_BRACKET: return "<";
			case LEFT_ANGLE_BRACKET_EQUAL: return "<=";
			case RIGHT_ANGLE_BRACKET: return ">";
			case RIGHT_ANGLE_BRACKET_EQUAL: return ">=";
			case ASTERISK: return "*";
			case SLASH: return "/";
			case PLUS: return "+";
			case MINUS: return "-";
			case EXCLAMATION: return "!";
			case TILDE: return "~";
			default: {
				// TODO
				BREAKPOINT();

				return "";
			}
		}
	}
}