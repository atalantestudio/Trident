// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Sample3D/Layer/Forward/ForwardLayer.hpp"

namespace Trident {
	void Sample3DForwardLayer::createPlayerEntity() {
		constexpr float32 m = 70.0f;

		const float32 ww = PLAYER_SCALE.x * PLAYER_SCALE.x;
		const float32 hh = PLAYER_SCALE.y * PLAYER_SCALE.y;
		const float32 dd = PLAYER_SCALE.z * PLAYER_SCALE.z;
		const float32 m_12 = (1.0f / 12.0f) * m;
		const mat3x3<float32> I = {
			m_12 * (hh + dd), 0.0f,             0.0f,
			0.0f,             m_12 * (ww + dd), 0.0f,
			0.0f,             0.0f,             m_12 * (ww + hh),
		};

		playerEntity = scene->ecs.createEntity();

		Geometry3DComponent geometryComponent;
		geometryComponent.geometryIndex = max<uint32>();
		geometryComponent.hasHull = true;
		geometryComponent.hull.centroid = {0.0f, 0.0f, 0.0f};
		geometryComponent.hull.radius = length(PLAYER_SCALE * 0.5f);
		geometryComponent.hull.vertices = playerHullVertices;
		geometryComponent.hull.edges = playerHullEdges;
		geometryComponent.hull.faces = playerHullFaces;
		geometryComponent.hull.faceEdgeIndices = playerHullFaceEdgeIndices;
		geometryComponent.hull.lineIndexRange.offset = static_cast<uint16>(scene->getAsset()->hullLineIndices.count()) - 24;
		geometryComponent.hull.lineIndexRange.count = 24;
		geometryComponent.hull.type = Hull3DType::POLYGON;

		RigidBody3DComponent rigidBodyComponent;
		rigidBodyComponent.position = {0.0f, 64.0f, 0.0f};
		rigidBodyComponent.rotation = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.scale = 1.0f;
		rigidBodyComponent.linearVelocity = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.angularVelocity = {0.0f, 0.0f, 0.0f};

		// TODO(physics/3d/test)
		// rigidBodyComponent.inverseMass = 1.0f / m;
		rigidBodyComponent.inverseMass = 0.0f;
		// rigidBodyComponent.inverseInertia = inverse(I);
		rigidBodyComponent.inverseInertia = {
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
		};

		rigidBodyComponent.friction = 0.2f;
		rigidBodyComponent.restitution = 0.0f;
		rigidBodyComponent.force = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.hasGroundEntity = false;
		rigidBodyComponent.updateOrientation();
		rigidBodyComponent.updateWorld(geometryComponent.hull.centroid);

		scene->ecs.setComponent<Geometry3DComponent>(playerEntity, geometryComponent);
		scene->ecs.setComponent<RigidBody3DComponent>(playerEntity, rigidBodyComponent);
	}
}