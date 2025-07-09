#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/Body/RigidBody.hpp"
#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	// TODO: Create "Runtime" directory for this kind of object?
	class Scene2D {
		public:
			explicit Scene2D() {
				ecs.registerComponent<ref<Body>>();
				ecs.registerComponent<ref<Geometry2D>>();
				ecs.registerComponent<ref<Material>>();
				ecs.registerComponent<PhysicalObject2D>();
			}

			uint32 getPhysicalObjectEntityCount() const {
				return ecs.getComponentPool<PhysicalObject2D>().componentCount;
			}

			Entity getLastEntity() const {
				return ecs.nextEntity - 1;
			}

			const ref<Body>& getBody(Entity entity) const {
				return ecs.getComponent<ref<Body>>(entity);
			}

			const ref<Geometry2D>& getGeometry(Entity entity) const {
				return ecs.getComponent<ref<Geometry2D>>(entity);
			}

			const ref<Material>& getMaterial(Entity entity) const {
				return ecs.getComponent<ref<Material>>(entity);
			}

			const PhysicalObject2D& getPhysicalObject(Entity entity) const {
				return ecs.getComponent<PhysicalObject2D>(entity);
			}

			Entity createPhysicalObjectEntity() {
				const Entity entity = ecs.createEntity();

				PhysicalObject2D physicalObject;

				ecs.setComponent<PhysicalObject2D>(entity, physicalObject);

				return entity;
			}

			void setBody(Entity entity, const ref<Body>& body) {
				ecs.setComponent<ref<Body>>(entity, body);
			}

			void setGeometry(Entity entity, const ref<Geometry2D>& geometry) {
				ecs.setComponent<ref<Geometry2D>>(entity, geometry);
			}

			void setMaterial(Entity entity, const ref<Material>& material) {
				ecs.setComponent<ref<Material>>(entity, material);
			}

			void updateTransform(Entity entity) {
				const ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(ecs.getComponent<ref<Body>>(entity));
				const ref<Geometry2D>& geometry = ecs.getComponent<ref<Geometry2D>>(entity);
				PhysicalObject2D& physicalObject = ecs.getComponent<PhysicalObject2D>(entity);

				physicalObject.transform =
					mat3x3<float32>::translation(-geometry->centerOfMass) *
					mat3x3<float32>::scaling(rigidBody->scale) *
					mat3x3<float32>::rotation(rigidBody->rotation) *
					mat3x3<float32>::translation(rigidBody->position);
			}

			void calculateMass(Entity entity) {
				const ref<Geometry2D>& geometry = ecs.getComponent<ref<Geometry2D>>(entity);
				const ref<Material>& material = ecs.getComponent<ref<Material>>(entity);
				const float32 volume = area(geometry->vertices);

				ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(ecs.getComponent<ref<Body>>(entity));
				rigidBody->inverseMass = 1.0f / (material->density * volume);
			}

		private:
			ECS ecs;
	};
}