// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "GeometryData.hpp"

namespace Trident {
	Geometry3DData DecodeFBXGeometry(view<uint8> inputData, uint32 identifier);
}