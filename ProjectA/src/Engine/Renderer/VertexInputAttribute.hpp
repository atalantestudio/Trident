#pragma once

#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"

namespace ProjectA {
	struct VertexInputAttribute {
		// Type of the attribute.
		VertexInputAttributeType type;

		// Whether the attribute is normalized.
		bool normalized;

		// Byte offset relative to the start of an element in the vertex input binding.
		uint32 offset;
	};
}