// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename T>
	inline ref<BufferView<T>> Buffer::createView(uint64 offset, uint64 size) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (offset + size > this->size) {
				ENGINE_ERROR("The viewing range is overflowing the buffer's size.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return createRef<BufferView<T>>(this, offset, size);
	}
}