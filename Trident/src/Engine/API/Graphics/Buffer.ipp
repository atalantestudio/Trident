// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename T>
	inline ref<BufferView<T>> Buffer::createView(uint64 offset, uint64 size) {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (size > this->size - offset) {
					VALIDATION_ERROR("size must be less than or equal to the size of the buffer - offset.");
				}
			}
		#endif

		return createRef<BufferView<T>>(this, offset, size);
	}
}