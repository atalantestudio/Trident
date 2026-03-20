// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"
#include "Engine/API/Physics/Joint2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	DEFINE_TAG_COMPONENT(DynamicEntityTag);
	DEFINE_TAG_COMPONENT(StaticEntityTag);

	class Scene2D : public Scene {
		public:
			explicit Scene2D() {
				ecs.registerComponent<RigidBody2D>();
				ecs.registerComponent<ref<Geometry2D>>();
				ecs.registerComponent<ref<PhysicsMaterial>>();
				ecs.registerComponent<Joint2D>();
				ecs.registerComponent<DynamicEntityTag>();
				ecs.registerComponent<StaticEntityTag>();
			}

			const RigidBody2D& getRigidBody(Entity entity) const {
				return ecs.getComponent<RigidBody2D>(entity);
			}

			RigidBody2D& getRigidBody(Entity entity) {
				return ecs.getComponent<RigidBody2D>(entity);
			}

			ref<Geometry2D> getGeometry(Entity entity) const {
				return ecs.getComponent<ref<Geometry2D>>(entity);
			}

			ref<PhysicsMaterial> getMaterial(Entity entity) const {
				return ecs.getComponent<ref<PhysicsMaterial>>(entity);
			}

			const Joint2D& getJoint(Entity entity) const {
				return ecs.getComponent<Joint2D>(entity);
			}

			Joint2D& getJoint(Entity entity) {
				return ecs.getComponent<Joint2D>(entity);
			}

			void createDynamicRigidBody(const RigidBody2DDescriptor& descriptor) {
				const Entity entity = createRigidBody(descriptor);
				const ref<PhysicsMaterial> material = getMaterial(entity);

				RigidBody2D& rigidBody = getRigidBody(entity);
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

				ecs.setComponent<DynamicEntityTag>(entity, {});
			}

			void createStaticRigidBody(const RigidBody2DDescriptor& descriptor) {
				const Entity entity = createRigidBody(descriptor);

				RigidBody2D& rigidBody = getRigidBody(entity);
				rigidBody.inverseMass = 0.0_kg;
				rigidBody.inverseInertia = 0.0_kg_m2;

				ecs.setComponent<StaticEntityTag>(entity, {});
			}

			void createJoint(const Joint2DDescriptor& descriptor) {
				const Entity entity = ecs.createEntity();
				const RigidBody2D& rigidBodyA = getRigidBody(descriptor.A);
				const RigidBody2D& rigidBodyB = getRigidBody(descriptor.B);
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

		private:
			void setGeometry(Entity entity, ref<Geometry2D> geometry) {
				ecs.setComponent<ref<Geometry2D>>(entity, geometry);
			}

			void setMaterial(Entity entity, ref<PhysicsMaterial> material) {
				ecs.setComponent<ref<PhysicsMaterial>>(entity, material);
			}

			Entity createRigidBody(const RigidBody2DDescriptor& descriptor) {
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
	};
}