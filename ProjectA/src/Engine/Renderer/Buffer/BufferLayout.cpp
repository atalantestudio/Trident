#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferLayout.hpp"

namespace ProjectA {
	BufferLayout::BufferLayout(const std::initializer_list<BufferAttribute>& attributes) :
		attributes(attributes)
	{
		updateStride();
	}

	std::vector<BufferAttribute>::iterator BufferLayout::begin() {
		return attributes.begin();
	}

	std::vector<BufferAttribute>::const_iterator BufferLayout::begin() const {
		return attributes.begin();
	}

	std::vector<BufferAttribute>::iterator BufferLayout::end() {
		return attributes.end();
	}

	std::vector<BufferAttribute>::const_iterator BufferLayout::end() const {
		return attributes.end();
	}

	void BufferLayout::updateStride() {
		stride = 0;

		for (BufferAttribute& attribute : attributes) {
			attribute.offset = stride;

			stride += attribute.size;
		}
	}
}