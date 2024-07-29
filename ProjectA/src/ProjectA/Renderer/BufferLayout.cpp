#include "pch.hpp"
#include "BufferLayout.hpp"

namespace ProjectA {
	void BufferLayout::calculateOffsetsAndStride() {
		stride = 0;

		for (auto& element : elements) {
			element.offset = stride;

			stride += element.size;
		}
	}

	BufferLayout::BufferLayout() {}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : elements(elements) {
		calculateOffsetsAndStride();
	}

	std::vector<BufferElement>::iterator BufferLayout::begin() {
		return elements.begin();
	}

	std::vector<BufferElement>::iterator BufferLayout::end() {
		return elements.end();
	}

	std::vector<BufferElement>::const_iterator BufferLayout::begin() const {
		return elements.begin();
	}

	std::vector<BufferElement>::const_iterator BufferLayout::end() const {
		return elements.end();
	}
}