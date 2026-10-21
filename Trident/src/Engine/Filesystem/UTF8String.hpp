// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class UTF8String {
		public:
			explicit UTF8String() = default;

			explicit UTF8String(uint32 code, uint8 byteSize);

			uint64 getByteSize() const;

			void insert(const UTF8String& insertion);

			void moveCursor(sint32 characterCount);
			void markCursorForSelection();

			void removeSelection();

			operator view<char8>() const;

		private:
			std::string string;

		public:
			/// Offset of the cursor in bytes.
			uint64 cursorOffset;
			uint64 selectionMarkerOffset;
	};
}