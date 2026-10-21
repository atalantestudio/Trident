// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
	UTF8String::UTF8String(uint32 code, uint8 byteSize) :
		string(pointer_cast<const char8*>(&code), byteSize),
		selected(false),
		cursorOffset(string.size()),
		selectionMarkerOffset(cursorOffset)
	{}

	uint64 UTF8String::getByteSize() const {
		return string.size();
	}

	bool UTF8String::hasSelection() const {
		return selected;
	}

	void UTF8String::insert(const UTF8String& insertion) {
		string.insert(cursorOffset, insertion.string.data());

		cursorOffset += insertion.getByteSize();
		selected = false;
	}

	void UTF8String::select() {
		selectionMarkerOffset = cursorOffset;
		selected = true;
	}

	void UTF8String::seek(uint64 byteOffset) {
		cursorOffset = byteOffset;
		selected = false;
	}

	void UTF8String::moveCursor(sint32 characterCount) {
		sint32 i = 0;

		while (i < abs(characterCount)) {
			if (((string[cursorOffset] >> 6) & 0b11) != 0b10) {
				i += 1;
			}

			if (characterCount < 0) {
				cursorOffset -= 1;
			}
			else {
				cursorOffset += 1;
			}
		}

		selected = false;
	}

	void UTF8String::removeSelection() {
		const uint64 selectionStartOffset = min(cursorOffset, selectionMarkerOffset);
		const uint64 selectionEndOffset = max(cursorOffset, selectionMarkerOffset);

		string.erase(selectionStartOffset, selectionEndOffset - selectionStartOffset);

		selected = false;
	}

	UTF8String::operator view<char8>() const {
		return view<char8>(string.data(), string.size());
	}
}