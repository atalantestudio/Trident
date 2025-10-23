#pragma once

namespace ProjectA {
	// NOTE: This enum is mapped to vscode.CompletionItemKind.
	enum class CompletionItemKind : uint8 {
		TEXT = 0,
		FUNCTION = 2,
		FIELD = 4,
		VARIABLE = 5,
		UNIT = 10,
		VALUE = 11,
		KEYWORD = 13,

		// TODO: Add?
		// COLOR = 15,

		FILE = 16,
		REFERENCE = 17,
		FOLDER = 18,
		CONSTANT = 20,
		STRUCTURE = 21,
		OPERATOR = 23,
		TEMPLATE_ARGUMENT = 24,
	};
}