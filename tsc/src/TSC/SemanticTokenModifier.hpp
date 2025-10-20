#pragma once

namespace ProjectA {
	enum SemanticTokenModifier : uint8 {
		// TODO
		// DEFINITION, // For definitions of symbols, for example, in header files.

		CONSTANT = BIT(0),

		// TODO
		// MODIFICATION, // For variable references where the variable is assigned to.

		PREDEFINED = BIT(1), // For symbols that are part of the standard library.
	};

	AS_FLAG(SemanticTokenModifier);
}