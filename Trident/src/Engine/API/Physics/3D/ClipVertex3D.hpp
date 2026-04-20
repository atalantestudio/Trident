// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/ContactID3D.hpp"

namespace Trident {
	struct ClipVertex3D {
		vec3<float32> position;
		float32 separation;
		ContactID3D id;
	};
}