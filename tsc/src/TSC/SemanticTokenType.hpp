#pragma once

namespace ProjectA {
	enum class SemanticTokenType : uint8 {
		// Identifiers that reference annotations.
		// The annotation square brackets are included in the semantic token.
		ANNOTATION,

		// Tokens that represent a comment.
		COMMENT,

		// TODO: Also use in function calls?
		// Identifiers that declare a function.
		FUNCTION,

		// Identifiers that declare or reference a local or global variable or member.
		VARIABLE,

		// Identifiers that declare or reference a structure member variable.
		MEMBER,

		// Identifiers that declare or reference a function parameter/argument.
		PARAMETER,

		// Identifiers that declare or reference a structure type.
		STRUCTURE,

		// Tokens that represent a language keyword or type.
		KEYWORD,

		// Tokens that represent a number literal.
		NUMBER,

		// Tokens that represent an operator.
		OPERATOR,

		// Tokens that represent a string literal.
		STRING,
	};
}