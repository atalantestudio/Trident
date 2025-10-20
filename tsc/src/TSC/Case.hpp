#pragma once

namespace ProjectA {
	static bool isPascalCase(const char8* string) {
		const char8* character = string;

		// Check that the first letter is uppercase.
		while (*character != '\0') {
			if (*character >= '0' && *character <= '9') {
				character += 1;

				continue;
			}

			if (*character < 'A' || *character > 'Z') {
				return false;
			}

			break;
		}

		// Check that there is no '_' in the remaining characters.
		while (*character != '\0') {
			if (*character == '_') {
				return false;
			}

			character += 1;
		}

		return true;
	}

	static bool isScreamingSnakeCase(const char8* string) {
		const char8* character = string;

		while (*character != '\0') {
			if (!((*character >= 'A' && *character <= 'Z') || (*character >= '0' && *character <= '9') || *character == '_')) {
				return false;
			}

			character += 1;
		}

		return true;
	}
}