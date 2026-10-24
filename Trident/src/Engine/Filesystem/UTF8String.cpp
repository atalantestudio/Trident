// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
	static bool isWordCharacter(char8 character) {
		return !(
			character == ' ' ||
			character == '"' ||
			character == '\'' ||
			character == ',' ||
			character == '-' ||
			character == '.' ||
			character == '/' ||
			character == '\\'
		);
	}

	UTF8String::UTF8String(view<char8> text) :
		string(pointer_cast<const char8*>(&text[0]), text.count()),
		selected(false),
		cursorOffset(string.size()),
		selectionMarkerOffset(0)
	{}

	UTF8String::UTF8String(uint32 code, uint8 byteSize) :
		string(pointer_cast<const char8*>(&code), byteSize),
		selected(false),
		cursorOffset(string.size()),
		selectionMarkerOffset(0)
	{}

	uint64 UTF8String::getByteSize() const {
		return string.size();
	}

	bool UTF8String::hasSelection() const {
		return selected;
	}

	view<char8> UTF8String::getSelection() const {
		uint64 startOffset = cursorOffset;
		uint64 endOffset = selectionMarkerOffset;

		if (cursorOffset > selectionMarkerOffset) {
			startOffset = selectionMarkerOffset;
			endOffset = cursorOffset;
		}

		return view<char8>(&string[startOffset], endOffset - startOffset);
	}

	void UTF8String::seek(uint64 byteOffset) {
		cursorOffset = byteOffset;
	}

	void UTF8String::jumpLeft(uint32 characterCount) {
		uint32 characterIndex = 0;

		while (cursorOffset > 0) {
			const char8 character = string[cursorOffset - 1];

			if (((character >> 6) & 0b11) != 0b10) {
				characterIndex += 1;
			}

			cursorOffset -= 1;

			if (characterIndex == characterCount) {
				break;
			}
		}
	}

	void UTF8String::jumpRight(uint32 characterCount) {
		uint32 characterIndex = 0;

		while (cursorOffset < string.size()) {
			const char8 character = string[cursorOffset];

			if (((character >> 6) & 0b11) != 0b10) {
				if (characterIndex == characterCount) {
					break;
				}

				characterIndex += 1;
			}

			cursorOffset += 1;
		}
	}

	void UTF8String::jumpWordStart() {
		char8 character = ' ';

		if (cursorOffset > 0) {
			character = string[cursorOffset - 1];
		}

		while (cursorOffset > 0) {
			char8 previousCharacter = ' ';

			if (cursorOffset > 1) {
				previousCharacter = string[cursorOffset - 2];
			}

			if (((character >> 6) & 0b11) == 0b10) {
				character = previousCharacter;
				cursorOffset -= 1;

				continue;
			}

			if (!isWordCharacter(previousCharacter) && isWordCharacter(character)) {
				cursorOffset -= 1;

				break;
			}

			character = previousCharacter;
			cursorOffset -= 1;
		}
	}

	void UTF8String::jumpWordEnd() {
		char8 character = string[cursorOffset];
		
		while (cursorOffset < string.size()) {
			char8 nextCharacter = ' ';

			if (cursorOffset < string.size() - 1) {
				nextCharacter = string[cursorOffset + 1];
			}

			if (((character >> 6) & 0b11) == 0b10) {
				character = nextCharacter;
				cursorOffset += 1;

				continue;
			}

			if (isWordCharacter(character) && !isWordCharacter(nextCharacter)) {
				cursorOffset += 1;

				break;
			}

			character = nextCharacter;
			cursorOffset += 1;
		}
	}

	void UTF8String::select() {
		selectionMarkerOffset = cursorOffset;
		selected = true;
	}

	void UTF8String::unselect() {
		selected = false;
	}

	void UTF8String::insert(const UTF8String& insertion) {
		if (selected) {
			uint64 startOffset = cursorOffset;
			uint64 endOffset = selectionMarkerOffset;

			if (cursorOffset > selectionMarkerOffset) {
				startOffset = selectionMarkerOffset;
				endOffset = cursorOffset;
			}

			string.replace(startOffset, endOffset - startOffset, insertion.string.data());

			unselect();

			cursorOffset = startOffset;
		}
		else {
			string.insert(cursorOffset, insertion.string.data());
		}

		cursorOffset += insertion.getByteSize();
	}

	void UTF8String::removeSelection() {
		uint64 startOffset = cursorOffset;
		uint64 endOffset = selectionMarkerOffset;

		if (cursorOffset > selectionMarkerOffset) {
			startOffset = selectionMarkerOffset;
			endOffset = cursorOffset;
		}

		string.erase(startOffset, endOffset - startOffset);

		unselect();

		cursorOffset = startOffset;
	}

	UTF8String::operator view<char8>() const {
		return view<char8>(string.data(), string.size());
	}
}