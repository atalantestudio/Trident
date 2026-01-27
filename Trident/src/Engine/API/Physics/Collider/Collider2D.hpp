#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/API/Physics/VisualizationData.hpp"
#endif

namespace Trident {
	typedef bool (*Collider2D)(
		const RigidBody2D& A,
		const RigidBody2D& B,
		ContactManifold2D* contactManifold
		#ifdef BUILD_MODE_DEBUG
			, VisualizationData2D* visualizationData
		#endif
	);
}