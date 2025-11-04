#pragma once

namespace ProjectA {
	static bool isCamelCaseOrSnakeCase(const std::string& characters) {
		return characters[0] >= 'a' && characters[0] <= 'z';
	}

	static bool isPascalCase(const std::string& characters) {
		// Check that the first letter is uppercase.
		if (characters[0] < 'A' || characters[0] > 'Z') {
			return false;
		}

		// Check that there is no '_' in the remaining characters.
		uint32 characterIndex = 1;

		while (characterIndex < characters.size()) {
			const char8 character = characters[characterIndex];

			if (character == '_') {
				return false;
			}

			characterIndex += 1;
		}

		return true;
	}

	static bool isScreamingSnakeCase(const std::string& characters) {
		uint32 characterIndex = 0;

		while (characterIndex < characters.size()) {
			const char8 character = characters[characterIndex];

			if (!((character >= 'A' && character <= 'Z') || (character >= '0' && character <= '9') || character == '_')) {
				return false;
			}

			characterIndex += 1;
		}

		return true;
	}
}