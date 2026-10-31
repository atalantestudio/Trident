// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/API/Physics/3D/Base.hpp"
#include "Engine/API/Physics/3D/ClipVertex3D.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"

namespace Trident {
	/// The Sutherland-Hodgman algorithm in 3D.
	/// 
	/// See:
	/// - https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
	/// - https://alamot.github.io/sutherland_hodgman_algorithm
	/// - https://www.flipcode.com/archives/Real-time_3D_Clipping_Sutherland-Hodgeman.shtml
	pool<ClipVertex3D, 8> SutherlandHodgman3D(const RigidBody3DComponent& referenceBody, const Hull3D& referenceHull, uint16 referenceFaceIndex, pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> incidentFaceVertices);
}