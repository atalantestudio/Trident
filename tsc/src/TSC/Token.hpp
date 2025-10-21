#pragma once

#include "Position.hpp"
#include "TokenType.hpp"

namespace ProjectA {
	// TODO: Find a way to serialize without having to write getters.
	class Token {
		public:
			uint32 getTypeIndex() const {
				return type;
			}

			uint32 getLineIndex() const {
				return start.lineIndex;
			}

			uint32 getCharacterOffset() const {
				return start.characterOffset;
			}

			uint32 getCharacterCount() const {
				return characterCount;
			}

			const std::string& getValue() const {
				return value;
			}

			Position getEnd() const {
				Position end{};
				end.lineIndex = start.lineIndex;
				end.characterOffset = start.characterOffset + characterCount;

				return end;
			}

		public:
			uint32 type;
			Position start;
			uint32 characterCount;

			// TODO: Replace value by an offset into a buffer stored in CompilationResult.
			std::string value;
			// uint32 identifierBufferOffset = ~0;
	};

	inline const std::string getTokenString(const Token& token) {
		switch (token.type) {
			case KEYWORD_AND: return "AND";
			case KEYWORD_BREAK: return "BREAK";
			case KEYWORD_IF: return "IF";
			case KEYWORD_NEXT: return "NEXT";
			case KEYWORD_OR: return "OR";
			case KEYWORD_RETURN: return "RETURN";
			case KEYWORD_WHILE: return "WHILE";
			case KEYWORD_XOR: return "XOR";
			case TYPE_FLOAT32: return "float32";
			case TYPE_FLOAT64: return "float64";
			case TYPE_SINT32: return "sint32";
			case TYPE_UINT32: return "uint32";
			case TYPE_VEC2: return "vec2";
			case TYPE_VEC3: return "vec3";
			case TYPE_VEC4: return "vec4";
			case TYPE_MAT2X2: return "mat2x2";
			case TYPE_MAT2X3: return "mat2x3";
			case TYPE_MAT2X4: return "mat2x4";
			case TYPE_MAT3X2: return "mat3x2";
			case TYPE_MAT3X3: return "mat3x3";
			case TYPE_MAT3X4: return "mat3x4";
			case TYPE_MAT4X2: return "mat4x2";
			case TYPE_MAT4X3: return "mat4x3";
			case TYPE_MAT4X4: return "mat4x4";
			case FUNCTION_NORMALIZED: return "\"normalized\"";
			case RESERVED_KEYWORD_COUNT: return "[reserved keyword]";
			case EQUAL: return "'='";
			case ASTERISK_EQUAL: return "'*='";
			case SLASH_EQUAL: return "'/='";
			case PLUS_EQUAL: return "'+='";
			case MINUS_EQUAL: return "'-='";
			case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL: return "'<<='";
			case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL: return "'>>='";
			case AMPERSAND_EQUAL: return "'&='";
			case CARET_EQUAL: return "'^='";
			case VERTICAL_BAR_EQUAL: return "'|='";
			case VERTICAL_BAR: return "'|'";
			case CARET: return "'^'";
			case AMPERSAND: return "'&'";
			case LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET: return "'<<'";
			case RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET: return "'>>'";
			case EQUAL_EQUAL: return "'=='";
			case EXCLAMATION_EQUAL: return "'!='";
			case LEFT_ANGLE_BRACKET: return "'<'";
			case LEFT_ANGLE_BRACKET_EQUAL: return "'<='";
			case RIGHT_ANGLE_BRACKET: return "'>'";
			case RIGHT_ANGLE_BRACKET_EQUAL: return "'>='";
			case ASTERISK: return "'*'";
			case SLASH: return "'/'";
			case PLUS: return "'+'";
			case MINUS: return "'-'";
			case EXCLAMATION: return "'!'";
			case TILDE: return "'~'";
			case COMMA: return "','";
			case DOT: return "'.'";
			case LEFT_CURLY_BRACE: return "'{'";
			case LEFT_PARENTHESIS: return "'('";
			case LEFT_SQUARE_BRACKET: return "'['";
			case RIGHT_CURLY_BRACE: return "'}'";
			case RIGHT_PARENTHESIS: return "')'";
			case RIGHT_SQUARE_BRACKET: return "']'";
			case END_OF_FILE: return "EOF";
			case END_OF_LINE: return "EOL";
			case LITERAL_FLOATING_POINT:
			case LITERAL_INTEGER_BASE_2:
			case LITERAL_INTEGER_BASE_8:
			case LITERAL_INTEGER_BASE_10:
			case LITERAL_INTEGER_BASE_16:
			case USER_DEFINED:
			default: {
				return token.value;
			}
		}
	}

	inline const std::string getOperatorTokenString(const Token& token) {
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
				ENGINE_ASSERT(false, "Invalid type.");

				return "???";
			}
		}
	}
}