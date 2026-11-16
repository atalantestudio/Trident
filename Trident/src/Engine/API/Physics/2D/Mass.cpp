// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Mass.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"

namespace Trident {
	MassProperties2D calculateMassProperties(const Shape2D& shape) {
		float32 m;
		float32 I;

		switch (shape.type) {
			case ShapeType2D::CIRCLE: {
				const float32 r = shape.circle.radius;
				const float32 rr = r * r;

				m = shape.density * pi<float32>() * rr;
				I = 0.5f * m * rr;

				break;
			}
			case ShapeType2D::CONVEX_HULL: {
				const vec2<float32>* vertices = shape.hull->getVertices();

				float32 Q = 0.0f;
				float32 area = 0.0f;
				uint32 i = 0;

				while (i < shape.hull->vertexCount) {
					const uint32 j = (i + 1) % shape.hull->vertexCount;

					const vec2<float32> Pi = vertices[i] - shape.hull->centroid;
					const vec2<float32> Pj = vertices[j] - shape.hull->centroid;

					const float32 Qi = lengthSquared(Pi) + dot(Pi, Pj) + lengthSquared(Pj);
					const float32 a = parallelogramArea(Pj, Pi) * 0.5f;

					Q += Qi * a;
					area += a;
					i += 1;
				}

				m = shape.density * area;
				I = m * (Q / (6.0f * area));

				break;
			}
			default: {
				DEBUG_ASSERT(false);

				break;
			}
		}

		MassProperties2D massProperties;
		massProperties.mass = m;
		massProperties.inertia = I;

		return massProperties;
	}

	void updateMassProperties(const Shape2D& shape, RigidBody2D& rigidBody) {
		DEBUG_ASSERT(rigidBody.type == RigidBodyType::DYNAMIC);

		const MassProperties2D massProperties = calculateMassProperties(shape);

		rigidBody.inverseMass = 1.0f / massProperties.mass;
		rigidBody.inverseInertia = 1.0f / massProperties.inertia;
	}
}