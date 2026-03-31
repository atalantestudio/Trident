// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Tag.hpp"

namespace Trident {
	Scene2D::Scene2D(ref<Scene2DAsset> sceneAsset) {
		// TODO: Load scene from asset file.
		/* const view<GeometryComponent> geometryComponents = sceneAsset->getGeometryComponents();

		Entity entity = 0;

		while (entity < sceneAsset->getEntityCount()) {
			ecs.createEntity();
			ecs.setComponent<GeometryComponent>(entity, geometryComponents[entity]);

			entity += 1;
		}

		for (const CachedRigidBody2D& rigidBody : sceneAsset->getRigidBodies()) {
			const GeometryComponent geometryComponent = ecs.getComponent<GeometryComponent>(rigidBody.entity);
			const Geometry2D& geometry = sceneAsset->getGeometries()[geometryComponent.geometryIndex];
			const Hull2D& hull = sceneAsset->getHulls()[geometry.getHullIndex()];

			ecs.setComponent<RigidBody2D>(rigidBody.entity, RigidBody2D::fromCached(rigidBody, hull.getCentroid()));
			ecs.setTag<DynamicEntityTag>(rigidBody.entity);
		} */

		// TODO(ecs/scene-components):
		// - MaterialReferenceComponent
		// - RigidBody3DComponent
		// - Transform2DComponent/Transform3DComponent for static meshes?

		ecs.registerComponentPool<GeometryComponent>();
		ecs.registerComponentPool<RigidBody2DComponent>();

		// TODO: Restore joint constraints.
		// ecs.registerComponentPool<Joint2D>();

		ecs.registerTagPool<DynamicEntityTag>();
		ecs.registerTagPool<StaticEntityTag>();
	}

	// TODO: Restore joint constraints.
	/* void Scene2D::createJoint(const Joint2DDescriptor& descriptor) {
		const Entity entity = ecs.createEntity();
		const RigidBody2D& rigidBodyA = ecs.getComponent<RigidBody2D>(descriptor.A);
		const RigidBody2D& rigidBodyB = ecs.getComponent<RigidBody2D>(descriptor.B);
		const mat2x2<float32> RA = transpose(mat2x2<float32>::rotation(rigidBodyA.rotation));
		const mat2x2<float32> RB = transpose(mat2x2<float32>::rotation(rigidBodyB.rotation));

		Joint2D joint{};
		joint.A = descriptor.A;
		joint.B = descriptor.B;
		joint.localAnchorA = (descriptor.anchor - rigidBodyA.position) * RA;
		joint.localAnchorB = (descriptor.anchor - rigidBodyB.position) * RB;
		joint.biasFactor = descriptor.biasFactor;
		joint.softness = descriptor.softness;

		ecs.setComponent<Joint2D>(entity, joint);
	} */
}