#include "pch.hpp"

#include "Engine/Renderer/VertexInputBinding.hpp"

namespace ProjectA {
	VertexInputBinding::VertexInputBinding(InputRate inputRate) :
		inputRate(inputRate)
	{}

	InputRate VertexInputBinding::getInputRate() const {
		return inputRate;
	}

	const chain<VertexInputAttribute>& VertexInputBinding::getAttributes() const {
		return attributes;
	}

	uint32 VertexInputBinding::getStride() const {
		return stride;
	}
}