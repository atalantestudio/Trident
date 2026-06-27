// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

namespace Trident {
	template<typename T>
	inline BufferView<T>::BufferView(const BaseBuffer* buffer, uint64 offset, uint64 size) :
		buffer(buffer),
		offset(offset),
		size(size)
	{}

	/// Returns the viewed buffer.
	template<typename T>
	inline const BaseBuffer* BufferView<T>::getBuffer() const {
		return buffer;
	}

	/// Returns the offset in bytes of the view.
	template<typename T>
	inline uint64 BufferView<T>::getOffset() const {
		return offset;
	}

	/// Returns the size in bytes of the view.
	template<typename T>
	inline uint64 BufferView<T>::getSize() const {
		return size;
	}

	/// Maps the view's range to host memory.
	template<typename T>
	inline T* BufferView<T>::map() const {
		return static_cast<T*>(buffer->map(offset, size));
	}
}