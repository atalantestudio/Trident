#pragma once

namespace ProjectA {
	static bool isCamelCaseOrSnakeCase(const char8* characters, uint32 characterCount) {
		return characters[0] >= 'a' && characters[0] <= 'z';
	}

	static bool isPascalCase(const char8* characters, uint32 characterCount) {
		// Check that the first letter is uppercase.
		if (characters[0] < 'A' || characters[0] > 'Z') {
			return false;
		}

		// Check that there is no '_' in the remaining characters.
		uint32 characterIndex = 1;

		while (characterIndex < characterCount) {
			const char8 character = characters[characterIndex];

			if (character == '_') {
				return false;
			}

			characterIndex += 1;
		}

		return true;
	}

	static bool isScreamingSnakeCase(const char8* characters, uint32 characterCount) {
		uint32 characterIndex = 0;

		while (characterIndex < characterCount) {
			const char8 character = characters[characterIndex];

			if (!((character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == '_')) {
				return false;
			}

			characterIndex += 1;
		}

		return true;
	}
}