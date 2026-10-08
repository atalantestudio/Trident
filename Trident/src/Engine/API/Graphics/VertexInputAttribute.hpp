// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"

namespace Trident {
	struct VertexInputAttribute {
		/// Format of the attribute.
		VertexInputAttributeFormat format;

		/// Offset relative to the start of the attribute in the input binding, in bytes.
		uint32 offset;
	};
}