// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/VertexInputBinding.hpp"

namespace Trident {
	VertexInputBinding::VertexInputBinding(InputRate inputRate, bool pad) :
		inputRate(inputRate),
		pad(pad)
	{}

	InputRate VertexInputBinding::getInputRate() const {
		return inputRate;
	}

	view<VertexInputAttribute> VertexInputBinding::getAttributes() const {
		return view<VertexInputAttribute>(attributes.data(), attributes.size());
	}

	uint32 VertexInputBinding::getMaxAlignment() const {
		return maxAlignment;
	}

	template<>
	void VertexInputBinding::registerAttribute<mat2x2<float32>>() {
		registerAttribute<vec2<float32>>();
		registerAttribute<vec2<float32>>();
	}

	template<>
	void VertexInputBinding::registerAttribute<mat3x3<float32>>() {
		registerAttribute<vec3<float32>>();
		registerAttribute<vec3<float32>>();
		registerAttribute<vec3<float32>>();
	}

	template<>
	void VertexInputBinding::registerAttribute<mat4x4<float32>>() {
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
		registerAttribute<vec4<float32>>();
	}
}