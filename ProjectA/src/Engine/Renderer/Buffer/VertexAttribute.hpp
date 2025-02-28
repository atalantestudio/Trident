#pragma once

#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"

namespace ProjectA {
	struct VertexAttribute {
		// Vertex buffer binding number.
		uint32 binding;

		// Shader input location number.
		uint32 location;

		// Type of the attribute.
		VertexInputAttributeType type;

		// Whether the attribute is normalized.
		bool normalized;

		// Byte offset relative to the start of an element in the vertex input binding.
		uint32 offset;
	};
}