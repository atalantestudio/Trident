// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Collider/CircleCircle.hpp"
#include "Engine/API/Physics/Collider/CircleConvexHull.hpp"
#include "Engine/API/Physics/Collider/ConvexHullConvexHull.hpp"

namespace Trident {
	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Hull2DType::count)][static_cast<uint32>(Hull2DType::count)] = {
		{nullptr, nullptr, nullptr},
		{nullptr, CircleCircleCollider, CircleConvexHullCollider},
		{nullptr, nullptr, ConvexHullConvexHullCollider},
	};
}