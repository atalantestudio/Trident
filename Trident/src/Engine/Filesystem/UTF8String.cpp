// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
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

		while (cursorOffset < getByteSize()) {
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