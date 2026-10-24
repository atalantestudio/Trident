// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class UTF8String {
		public:
			explicit UTF8String() = default;

			UTF8String(view<char8> text);

			explicit UTF8String(uint32 code, uint8 byteSize);

			uint64 getByteSize() const;

			bool hasSelection() const;

			view<char8> getSelection() const;

			void seek(uint64 byteOffset);

			void jumpLeft(uint32 characterCount);
			void jumpRight(uint32 characterCount);

			void jumpWordStart();
			void jumpWordEnd();

			void select();
			void unselect();

			void insert(const UTF8String& insertion);

			void removeSelection();

			operator view<char8>() const;

		private:
			std::string string;
			bool selected;

		public:
			/// Offset of the cursor in bytes.
			uint64 cursorOffset;

			/// Selection marker offset in bytes.
			uint64 selectionMarkerOffset;
	};
}