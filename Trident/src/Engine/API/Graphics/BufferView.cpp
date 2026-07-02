// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/BufferView.hpp"

namespace Trident {
	BaseBufferView::BaseBufferView(Buffer* buffer, uint64 offset, uint64 size, uint32 stride) :
		buffer(buffer),
		offset(offset),
		size(size),
		stride(stride)
	{}

	Buffer* BaseBufferView::getBuffer() {
		return buffer;
	}

	uint64 BaseBufferView::getOffset() const {
		return offset;
	}

	uint64 BaseBufferView::getSize() const {
		return size;
	}

	uint32 BaseBufferView::getStride() const {
		return stride;
	}
}