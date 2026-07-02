// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename T>
	inline BufferView<T>::BufferView(Buffer* buffer, uint64 offset, uint64 size) :
		BaseBufferView(buffer, offset, size, sizeof(T))
	{}

	template<typename T>
	inline T* BufferView<T>::map() {
		return static_cast<T*>(buffer->map(offset, size));
	}
}