#include "pch.hpp"

#include "Engine/Renderer/Buffer/BufferLayout.hpp"

namespace ProjectA {
	void BufferLayout::defineBinding(const uint32 index, const InputRate inputRate) {
		VertexBinding vertexBinding{};
		vertexBinding.index = index;
		vertexBinding.stride = 0;
		vertexBinding.inputRate = inputRate;

		vertexBindings.emplace_back(vertexBinding);
	}
}