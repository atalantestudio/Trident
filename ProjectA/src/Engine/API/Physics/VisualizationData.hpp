#pragma once

#include "Engine/API/Physics/ContactManifold.hpp"
#include "Engine/API/Physics/GJK2D.hpp"
#include "Engine/API/Physics/SAT2D.hpp"

namespace ProjectA {
	struct VisualizationData2D {
		GJK2DOutput gjkOutput;
		SAT2DOutput satOutput;
		ContactManifold2D contactManifold;
	};
}