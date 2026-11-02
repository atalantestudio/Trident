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
		geometryComponent.hull.lineIndexRange.offset = 0;
		geometryComponent.hull.lineIndexRange.count = 0;
		geometryComponent.hull.type = Hull3DType::POLYGON;

		RigidBody3DComponent rigidBodyComponent;
		rigidBodyComponent.position = {0.0f, 3.0f, 0.0f};
		rigidBodyComponent.rotation = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.scale = 1.0f;
		rigidBodyComponent.linearVelocity = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.angularVelocity = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.inverseMass = 1.0f / m;
		rigidBodyComponent.inverseInertia = inverse(I);
		rigidBodyComponent.friction = 0.2f;
		rigidBodyComponent.restitution = 0.0f;
		rigidBodyComponent.force = {0.0f, 0.0f, 0.0f};
		rigidBodyComponent.groundEntity = max<Entity>();
		rigidBodyComponent.updateOrientation();
		rigidBodyComponent.updateWorld(geometryComponent.hull.centroid);

		scene->ecs.setComponent<Geometry3DComponent>(playerEntity, geometryComponent);
		scene->ecs.setComponent<RigidBody3DComponent>(playerEntity, rigidBodyComponent);
	}
}