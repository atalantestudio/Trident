// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class VisualizationMode : uint8 {
		/// Renders static scene geometry with the material shaders.
		STATIC_GEOMETRY_MATERIALS = BIT(0),

		/// Renders dynamic scene geometry with the material shaders.
		DYNAMIC_GEOMETRY_MATERIALS = BIT(1),

		/// Draws the wireframe of the rigid body hulls as yellow lines.
		HULL_WIREFRAMES = BIT(2),

		/// Draws the perimeter of the selected manifold as red lines.
		CONTACT_MANIFOLDS = BIT(3),
	};

	AS_FLAG(VisualizationMode);
}