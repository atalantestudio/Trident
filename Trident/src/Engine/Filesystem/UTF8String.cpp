// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
	UTF8String::UTF8String(uint32 code, uint8 byteSize) :
		string(pointer_cast<const char8*>(&code), byteSize),
		cursorOffset(string.size()),
		selectionMarkerOffset(cursorOffset)
	{}

	uint64 UTF8String::getByteSize() const {
		return string.size();
	}

	void UTF8String::insert(const UTF8String& insertion) {
		string.insert(cursorOffset, insertion.string.data());

		cursorOffset += insertion.getByteSize();
		selectionMarkerOffset = cursorOffset;
	}

	void UTF8String::markCursorForSelection() {
		selectionMarkerOffset = cursorOffset;
	}

	void UTF8String::moveCursor(sint32 characterCount) {
		sint32 i = 0;

		while (i < abs(characterCount)) {
			if (((string[cursorOffset - 1] >> 6) & 0b11) != 0b10) {
				i += 1;
			}

			if (characterCount < 0) {
				cursorOffset -= 1;
			}
			else {
				cursorOffset += 1;
			}
		}
	}

	void UTF8String::removeSelection() {
		string.erase(min(cursorOffset, selectionMarkerOffset), max(cursorOffset, selectionMarkerOffset));

		selectionMarkerOffset = cursorOffset;
	}

	UTF8String::operator view<char8>() const {
		return view<char8>(string.data(), string.size());
	}
}