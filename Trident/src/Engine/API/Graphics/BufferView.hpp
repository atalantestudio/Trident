// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class BaseBuffer;

	template<typename T>
	class BufferView {
		public:
			explicit BufferView(
				/// The viewed buffer.
				const BaseBuffer* buffer,

				/// The offset in bytes of the view.
				uint64 offset,

				/// The size in bytes of the view.
				uint64 size
			);

			/// Returns the viewed buffer.
			const BaseBuffer* getBuffer() const;

			/// Returns the offset in bytes of the view.
			inline uint64 getOffset() const;

			/// Returns the size in bytes of the view.
			inline uint64 getSize() const;

			/// Maps the view's range to host memory.
			inline T* map() const;

		private:
			const BaseBuffer* buffer;
			uint64 offset;
			uint64 size;
	};
}