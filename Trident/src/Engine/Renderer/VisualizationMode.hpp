// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class VisualizationMode : uint8 {
		/// Renders all scene geometry with the material shaders.
		DEFAULT = BIT(0),

		/// Draws the wireframe of the rigid body hulls as yellow lines.
		HULL_WIREFRAMES = BIT(1),

		/// Draws the perimeter of the selected manifold as red lines.
		CONTACT_MANIFOLDS = BIT(2),
	};

	AS_FLAG(VisualizationMode);
}