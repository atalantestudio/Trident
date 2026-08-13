// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Buffer;

	class BaseBufferView {
		public:
			BaseBufferView() = default;

			explicit BaseBufferView(
				/// The viewed buffer.
				Buffer* buffer,

				/// The offset in bytes of the view.
				uint64 offset,

				/// The size in bytes of the view.
				uint64 size,

				/// The element stride.
				uint32 stride
			);

			/// Returns the viewed buffer.
			Buffer* getBuffer() const;

			/// Returns the offset in bytes of the view.
			uint64 getOffset() const;

			/// Returns the size in bytes of the view.
			uint64 getSize() const;

			/// Returns the element stride in bytes.
			uint32 getStride() const;

			// TODO(graphics/buffer/view): Remove?
			/// Maps the view's range to host memory.
			void* map();

		protected:
			// TODO(graphics/buffer-view/explicit-mapping)
			// Restore explicit buffer mapping. This will allow this member to be const.
			// The BufferView::map method must be replaced with BufferView::getMappedRange, which returns a typed pointer with the view's offset.
			Buffer* buffer;

			uint64 offset;
			uint64 size;
			uint32 stride;
	};

	template<typename T>
	class BufferView : public BaseBufferView {
		public:
			BufferView() = default;

			explicit BufferView(
				/// The viewed buffer.
				Buffer* buffer,

				/// The offset in bytes of the view.
				uint64 offset,

				/// The size in bytes of the view.
				uint64 size
			);

			/// Maps the view's range to host memory.
			inline T* map() const;
	};
}