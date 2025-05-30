#pragma once

#include "Engine/Renderer/VertexInputAttributeType.hpp"

namespace ProjectA {
	struct VertexInputAttribute {
		// Type of the attribute.
		VertexInputAttributeType type;

		// Whether the attribute is normalized.
		// Only used with the OpenGL abstraction.
		bool normalized;

		// Offset relative to the start of an element in the vertex input binding, in bytes.
		uint32 offset;
	};
}