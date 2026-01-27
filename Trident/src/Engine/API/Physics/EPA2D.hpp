// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/Simplex.hpp"

namespace Trident {
	static constexpr float32 EPA_2D_BIAS = 0.001f;

	struct EPA2DResult {
		vec2<float32> normal;
		float32 penetration;
	};

	EPA2DResult EPA2D(const Simplex2& simplex, const PhysicalObjectCopy2D& A, const PhysicalObjectCopy2D& B) {
		float32 minDistance = max<float32>();
		vec2<float32> minNormal = {0.0f, 0.0f};
		uint32 minIndex = 0;

		std::vector<vec2<float32>> polytope;

		for (uint32 vertexIndex = 0; vertexIndex < simplex.count; vertexIndex += 1) {
			polytope[vertexIndex] = simplex[vertexIndex].v;
		}

		while (minDistance == max<float32>()) {
			for (uint32 indexI = 0; indexI < polytope.size(); indexI += 1) {
				const uint32 indexJ = (indexI + 1) % polytope.size();

				const vec2<float32> i = polytope[indexI];
				const vec2<float32> j = polytope[indexJ];
				const vec2<float32> ij = j - i;

				vec2<float32> normal = normalized(perpendicularRight(ij));

				float32 distance = dot(normal, i);

				if (distance < 0.0f) {
					distance = -distance;
					normal = -normal;
				}

				if (distance < minDistance) {
					minDistance = distance;
					minNormal = normal;

					minIndex = indexJ;
				}
			}

			const vec2<float32> s = B.geometry->support(minNormal) - A.geometry->support(-minNormal);
			const float32 distanceS = dot(minNormal, s);

			// If the distance of the support point is greater than the minimum distance, reset the minimum distance.
			if (abs(distanceS - minDistance) > 0.001) {
				minDistance = max<float32>();
				polytope.emplace(polytope.begin() + minIndex, s);
			}
		}

		EPA2DResult result;
		result.penetration = minDistance + EPA_2D_BIAS;
		result.normal = minNormal * result.penetration;

		return result;
	}
}