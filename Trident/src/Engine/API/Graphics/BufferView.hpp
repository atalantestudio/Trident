// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class BaseBuffer;

	class BaseBufferView {
		public:
			explicit BaseBufferView(
				/// The viewed buffer.
				BaseBuffer* buffer,

				/// The offset in bytes of the view.
				uint64 offset,

				/// The size in bytes of the view.
				uint64 size,

				/// The element stride.
				uint32 stride
			);

			/// Returns the viewed buffer.
			BaseBuffer* getBuffer();

			/// Returns the offset in bytes of the view.
			uint64 getOffset() const;

			/// Returns the size in bytes of the view.
			uint64 getSize() const;

			/// Returns the element stride in bytes.
			uint32 getStride() const;

			/// Maps the view's range to host memory.
			void* map();

		protected:
			BaseBuffer* buffer;
			uint64 offset;
			uint64 size;
			uint32 stride;
	};

	template<typename T>
	class BufferView : public BaseBufferView {
		public:
			explicit BufferView(
				/// The viewed buffer.
				BaseBuffer* buffer,

				/// The offset in bytes of the view.
				uint64 offset,

				/// The size in bytes of the view.
				uint64 size
			);

			/// Maps the view's range to host memory.
			inline T* map();
	};
}