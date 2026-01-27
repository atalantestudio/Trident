// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PrimitiveTopology : uint8 {
		/// Specifies a list of point primitives.
		POINT_LIST,

		/// Specifies a list of line primitives.
		LINE_LIST,

		/// Specifies a list of line primitives where each consecutive line pair shares a vertex.
		LINE_STRIP,

		/// Specifies a list of triangle primitives.
		TRIANGLE_LIST,

		/// Specifies a list of triangle primitives where each consecutive triangle pair shares an edge.
		TRIANGLE_STRIP,

		// TODO
		// On Vulkan, TRIANGLE_FAN is only supported with an extension.
		// It will be added when the device is updated to automatically enable needed extensions.

		count,
	};
}