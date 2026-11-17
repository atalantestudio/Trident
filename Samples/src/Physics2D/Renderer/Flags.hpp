// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class PhysicsRendererFlags2D : uint8 {
		/// Indicates that the solid part of hulls will be rendered.
		/// Circle shapes aren't affected by this flag and always render as solid objects.
		RENDER_SOLID = BIT(0),

		/// Renders the hull wireframes.
		/// Circle shapes aren't affected by this flag.
		RENDER_WIREFRAME = BIT(1),

		/// Renders contact points as red dots.
		RENDER_CONTACTS = BIT(2),
	};

	AS_FLAG(PhysicsRendererFlags2D);
}