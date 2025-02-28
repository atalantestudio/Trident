#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferLayout.hpp"

namespace ProjectA {
	std::vector<VertexAttribute>::const_iterator BufferLayout::begin() const {
		return vertexAttributes.begin();
	}

	std::vector<VertexAttribute>::iterator BufferLayout::begin() {
		return vertexAttributes.begin();
	}

	std::vector<VertexAttribute>::const_iterator BufferLayout::end() const {
		return vertexAttributes.end();
	}

	std::vector<VertexAttribute>::iterator BufferLayout::end() {
		return vertexAttributes.end();
	}
}