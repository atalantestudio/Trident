// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/ECS/Entity.hpp"

namespace Trident {
	struct EntityPair {
		Entity A;
		Entity B;
	};

	inline bool operator<(EntityPair a, EntityPair b) {
		if (a.A < b.A) {
			return true;
		}

		if (a.A == b.A && a.B < b.B) {
			return true;
		}

		return false;
	}
}