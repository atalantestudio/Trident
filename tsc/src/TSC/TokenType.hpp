#pragma once

namespace ProjectA {
	enum TokenType : uint32 {
		KEYWORD_AND,
		KEYWORD_BREAK,
		KEYWORD_IF,

		// NOTE: The "include" keyword is handed by the Preprocessor class.
		KEYWORD_NEXT,
		KEYWORD_OR,
		KEYWORD_RETURN,
		KEYWORD_WHILE,
		KEYWORD_XOR,

		TYPE_FLOAT32,
		TYPE_FLOAT64,
		TYPE_SINT32,
		TYPE_UINT32,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_VEC4,
		TYPE_MAT2X2,
		TYPE_MAT2X3,
		TYPE_MAT2X4,
		TYPE_MAT3X2,
		TYPE_MAT3X3,
		TYPE_MAT3X4,
		TYPE_MAT4X2,
		TYPE_MAT4X3,
		TYPE_MAT4X4,

		FUNCTION_NORMALIZED,

		// Offsets and counters
		// NOTE: Should only be used to iterate over the identifiers defined above.
		RESERVED_KEYWORD_COUNT,

		// Assignment operator
		EQUAL,

		// Compound operators
		ASTERISK_EQUAL,
		SLASH_EQUAL,
		PLUS_EQUAL,
		MINUS_EQUAL,
		LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET_EQUAL,
		RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET_EQUAL,
		AMPERSAND_EQUAL,
		CARET_EQUAL,
		VERTICAL_BAR_EQUAL,

		// Bitwise operators
		VERTICAL_BAR,
		CARET,
		AMPERSAND,
		LEFT_ANGLE_BRACKET_LEFT_ANGLE_BRACKET,
		RIGHT_ANGLE_BRACKET_RIGHT_ANGLE_BRACKET,

		// Equality operators
		EQUAL_EQUAL,
		EXCLAMATION_EQUAL,

		// Relational operators
		LEFT_ANGLE_BRACKET,
		LEFT_ANGLE_BRACKET_EQUAL,
		RIGHT_ANGLE_BRACKET,
		RIGHT_ANGLE_BRACKET_EQUAL,

		// Multiplicative operators
		ASTERISK,
		SLASH,

		// Additive operators
		PLUS,
		MINUS,

		// Unary operators
		EXCLAMATION,
		TILDE,

		// Other
		COMMA,
		DOT,
		LEFT_CURLY_BRACE,
		LEFT_PARENTHESIS,
		LEFT_SQUARE_BRACKET,
		RIGHT_CURLY_BRACE,
		RIGHT_PARENTHESIS,
		RIGHT_SQUARE_BRACKET,

		// Special
		END_OF_FILE,
		END_OF_LINE,

		// Number literals
		LITERAL_FLOATING_POINT,
		LITERAL_INTEGER_BASE_2,
		LITERAL_INTEGER_BASE_8,
		LITERAL_INTEGER_BASE_10,
		LITERAL_INTEGER_BASE_16,

		// User-defined identifiers (structures, structure members, variables, constants, functions)
		// NOTE: All unique identifiers are indexed relatively to this value, e.g. the third user-defined identifier will have the token type USER_DEFINED + 2.
		USER_DEFINED,
	};

	static constexpr uint32 FIRST_TYPE_INDEX = TYPE_FLOAT32;
	static constexpr uint32 FIRST_TEMPLATE_ARGUMENT_INDEX = TYPE_FLOAT32;
	static constexpr uint32 FIRST_FUNCTION_INDEX = FUNCTION_NORMALIZED;

	static constexpr uint32 LAST_TYPE_INDEX = TYPE_MAT4X4;
	static constexpr uint32 LAST_TEMPLATE_ARGUMENT_INDEX = TYPE_UINT32;
	static constexpr uint32 LAST_FUNCTION_INDEX = FUNCTION_NORMALIZED;
	static constexpr uint32 LAST_KEYWORD_INDEX = LAST_TYPE_INDEX;

	static constexpr uint32 VECTOR_TEMPLATE_ARGUMENTS = BIT(TYPE_FLOAT32) | BIT(TYPE_FLOAT64) | BIT(TYPE_SINT32) | BIT(TYPE_UINT32);
	static constexpr uint32 MATRIX_TEMPLATE_ARGUMENTS = BIT(TYPE_FLOAT32) | BIT(TYPE_FLOAT64);

	static constexpr const char8* RESERVED_KEYWORDS[RESERVED_KEYWORD_COUNT] = {
		"and",
		"break",
		"if",
		"next",
		"or",
		"return",
		"while",
		"xor",

		"float32",
		"float64",
		"sint32",
		"uint32",
		"vec2",
		"vec3",
		"vec4",
		"mat2x2",
		"mat2x3",
		"mat2x4",
		"mat3x2",
		"mat3x3",
		"mat3x4",
		"mat4x2",
		"mat4x3",
		"mat4x4",

		"normalized",
	};
}