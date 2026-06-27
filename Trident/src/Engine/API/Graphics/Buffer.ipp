// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename T>
	inline ref<BufferView<T>> BaseBuffer::createView(uint64 offset, uint64 size) {
		return createRef<BufferView<T>>(this, offset, size);
	}
}