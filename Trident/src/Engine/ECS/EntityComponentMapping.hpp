// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Entity.hpp"

namespace Trident {
	template<typename ComponentType>
	struct EntityComponentMapping {
		Entity entity;
		ComponentType component;
	};
}