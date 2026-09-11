// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class InstanceFlags {
		/// Disables triangle face culling for this instance.
		DISABLE_TRIANGLE_FACE_CULLING = BIT(0),

		/// Inverts front and back facings.
		/// By default, a triangle is front-facing if its vertices are laid out in a clockwise order.
		FLIP_TRIANGLE_FACING = BIT(1),

		/// Forces the instance to be opaque, regardless of the geometry flags.
		FORCE_OPAQUE = BIT(2),

		/// Forces the instance to not be opaque, regardless of the geometry flags.
		FORCE_NON_OPAQUE = BIT(3),
	};

	AS_FLAG(InstanceFlags);
}