// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/MassProperties.hpp"
#include "Engine/API/Physics/2D/Shape/Shape.hpp"

namespace Trident {
	MassProperties2D calculateMassProperties(const Shape2D& shape);
}