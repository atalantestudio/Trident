// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"
#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/API/Scene/Scene2D.hpp"

namespace Trident {
	Scene2D::Scene2D() {
		ecs.registerComponent<RigidBody2D>();
		ecs.registerComponent<ref<Geometry2D>>();
		ecs.registerComponent<ref<PhysicsMaterial>>();
		ecs.registerComponent<ref<Constraint2D>>();
		ecs.registerComponent<Joint2D>();

		ecs.registerComponent<DynamicEntityTag>();
		ecs.registerComponent<StaticEntityTag>();
	}

	void Scene2D::createGhostRigidBody(const RigidBody2DDescriptor& descriptor) {
		const Entity entity = createRigidBody(descriptor);
		const ref<PhysicsMaterial> material = ecs.getComponent<ref<PhysicsMaterial>>(entity);

		RigidBody2D& rigidBody = getComponent<RigidBody2D>(entity);
		rigidBody.linearVelocity = descriptor.linearVelocity;
		rigidBody.angularVelocity = descriptor.angularVelocity;

		const float32 m = descriptor.mass;

		if (m == 0.0_kg) {
			rigidBody.inverseMass = 0.0_kg;
			rigidBody.inverseInertia = 0.0f;
		}
		else {
			rigidBody.inverseMass = 1.0f / m;

			// TODO(stable-simulation)
			const float32 area = rigidBody.scale[0] * rigidBody.scale[0] + rigidBody.scale[1] * rigidBody.scale[1];
			// const float32 area = descriptor.geometry->calculateMass(rigidBody.scale, material->density) * 2.0f;

			// TODO: Is this correct? See https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47.
			const float32 I = m * area / 12.0f;

			rigidBody.inverseInertia = 1.0f / I;
		}
	}

	void Scene2D::createDynamicRigidBody(const RigidBody2DDescriptor& descriptor) {
		createGhostRigidBody(descriptor);

		const Entity entity = getLastEntity();

		ecs.setComponent<DynamicEntityTag>(entity, {});
	}

	void Scene2D::createStaticRigidBody(const RigidBody2DDescriptor& descriptor) {
		const Entity entity = createRigidBody(descriptor);

		RigidBody2D& rigidBody = ecs.getComponent<RigidBody2D>(entity);
		rigidBody.inverseMass = 0.0_kg;
		rigidBody.inverseInertia = 0.0_kg_m2;

		ecs.setComponent<StaticEntityTag>(entity, {});
	}

	void Scene2D::createJoint(const Joint2DDescriptor& descriptor) {
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
	}

	void Scene2D::setGeometry(Entity entity, ref<Geometry2D> geometry) {
		ecs.setComponent<ref<Geometry2D>>(entity, geometry);
	}

	void Scene2D::setMaterial(Entity entity, ref<PhysicsMaterial> material) {
		ecs.setComponent<ref<PhysicsMaterial>>(entity, material);
	}

	Entity Scene2D::createRigidBody(const RigidBody2DDescriptor& descriptor) {
		const Entity entity = ecs.createEntity();

		setGeometry(entity, descriptor.geometry);

		// TODO: Set correct restitution.
		ref<ColorFillStrokeMaterial> material = createRef<ColorFillStrokeMaterial>();
		material->density = descriptor.density;
		material->fillColor = descriptor.fillColor.value;
		material->strokeColor = descriptor.strokeColor.value;

		ENGINE_ASSERT(material->density > 0.0f, "Invalid material density.");
		ENGINE_ASSERT(material->restitution >= 0.0f && material->restitution <= 1.0f, "Invalid material restitution.");

		setMaterial(entity, std::static_pointer_cast<PhysicsMaterial>(material));

		RigidBody2D rigidBody{};
		rigidBody.position = descriptor.position;
		rigidBody.rotation = descriptor.rotation;
		rigidBody.scale = descriptor.scale;
		rigidBody.centerOfMass = descriptor.geometry->centroid;
		rigidBody.friction = descriptor.friction;
		rigidBody.geometry = ecs.getComponent<ref<Geometry2D>>(entity);
		rigidBody.label = descriptor.label;

		rigidBody.updateTransform();

		ecs.setComponent<RigidBody2D>(entity, rigidBody);

		return entity;
	}
}