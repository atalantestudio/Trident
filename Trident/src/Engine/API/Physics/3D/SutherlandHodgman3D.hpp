// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/ClipVertex3D.hpp"
#include "Engine/API/Physics/3D/SAT3D.hpp"

namespace Trident {
	/// The Sutherland-Hodgman algorithm in 3D.
	/// 
	/// See:
	/// - https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
	/// - https://alamot.github.io/sutherland_hodgman_algorithm
	/// - https://www.flipcode.com/archives/Real-time_3D_Clipping_Sutherland-Hodgeman.shtml
	pool<ClipVertex3D, 8> SutherlandHodgman3D(const FaceQuery& faceQuery, uint16 faceIndex, pool<ClipVertex3D, MAX_FACE_VERTEX_COUNT> incidentFaceVertices, const pool<uint16, 8>& referenceSidePlaneIndices);
}