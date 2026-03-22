// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/Contact.hpp"

namespace Trident {
	struct Contact2DID;

	struct ClipVertex2D {
		vec2<float32> position;
		Contact2DID id;
	};

	// A vertex or edge of an object's geometry in world-space.
	struct Feature2D {
		Feature2D() {}

		// The number of vertices used.
		uint8 vertexCount;

		// The transformed vertices that compose the feature.
		union {
			vec2<float32> vertex;
			vec2<float32> edge[2];
		};
	};

	struct ClipFeature2D {
		ClipFeature2D() {}

		// The number of vertices used.
		uint8 vertexCount;

		// The transformed vertices that compose the feature.
		union {
			ClipVertex2D vertex;
			ClipVertex2D edge[2];
		};
	};
}