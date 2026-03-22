// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	class Constraint2D;
	struct Joint2DDescriptor;
	class Geometry2D;
	struct RigidBody2DDescriptor;
	struct PhysicsMaterial;

	DEFINE_TAG_COMPONENT(DynamicEntityTag);
	DEFINE_TAG_COMPONENT(StaticEntityTag);

	class Scene2D : public Scene {
		public:
			explicit Scene2D();

			template<typename ComponentType>
			const ComponentType& getComponent(Entity entity) const {
				return ecs.getComponent<ComponentType>(entity);
			}

			template<typename ComponentType>
			ComponentType& getComponent(Entity entity) {
				return ecs.getComponent<ComponentType>(entity);
			}

			/// Creates a rigid body without a collider.
			void createGhostRigidBody(const RigidBody2DDescriptor& descriptor);

			void createDynamicRigidBody(const RigidBody2DDescriptor& descriptor);

			void createStaticRigidBody(const RigidBody2DDescriptor& descriptor);

			template<typename ConstraintType, typename DescriptorType>
			void createConstraint(const DescriptorType& descriptor) {
				const Entity entity = ecs.createEntity();
				const ref<ConstraintType> constraint = createRef<ConstraintType>(descriptor);

				ecs.setComponent<ref<Constraint2D>>(entity, constraint);
			}

			void createJoint(const Joint2DDescriptor& descriptor);

		private:
			void setGeometry(Entity entity, ref<Geometry2D> geometry);

			void setMaterial(Entity entity, ref<PhysicsMaterial> material);

			Entity createRigidBody(const RigidBody2DDescriptor& descriptor);
	};
}