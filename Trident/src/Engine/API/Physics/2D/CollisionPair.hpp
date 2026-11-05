// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct CollisionPair2D {
		ShapeID2D shapeIdA;
		ShapeID2D shapeIdB;
	};

	inline bool operator<(CollisionPair2D a, CollisionPair2D b) {
		if (a.shapeIdA < b.shapeIdA) {
			return true;
		}

		if (a.shapeIdA == b.shapeIdA && a.shapeIdB < b.shapeIdB) {
			return true;
		}

		return false;
	}
}