#pragma once

#include "Engine/API/Geometry/Geometry2DType.hpp"
#include "Engine/API/Physics/Collider/Collider2D.hpp"
#include "Engine/API/Physics/Collider/CircleCircle.hpp"
#include "Engine/API/Physics/Collider/CircleConvexHull.hpp"
#include "Engine/API/Physics/Collider/ConvexHullConvexHull.hpp"

namespace ProjectA {
	static constexpr Collider2D COLLIDER_2D_TABLE[static_cast<uint32>(Geometry2DType::MAX_ENUM)][static_cast<uint32>(Geometry2DType::MAX_ENUM)] = {
		{CircleCircleCollider, CircleConvexHullCollider    },
		{nullptr,              ConvexHullConvexHullCollider},
	};
}