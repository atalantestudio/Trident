#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/Body/RigidBody.hpp"
#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	// TODO: Create "Runtime" directory for this kind of object?
	class Scene2D {
		private:
			struct DynamicEntityComponent {};
			struct StaticEntityComponent {};

		private:
			static constexpr uint32 LAYER_COUNT = 4;

		public:
			explicit Scene2D() {
				ecs.registerComponent<ref<Body2D>>();
				ecs.registerComponent<ref<Geometry2D>>();
				ecs.registerComponent<ref<Material>>();
				ecs.registerComponent<PhysicalObject2D>();
				ecs.registerComponent<DynamicEntityComponent>();
				ecs.registerComponent<StaticEntityComponent>();
			}

			const std::array<std::vector<Entity>, LAYER_COUNT>& getLayers() const {
				return layers;
			}

			uint32 getPhysicalObjectEntityCount() const {
				return ecs.getComponentPool<PhysicalObject2D>().componentCount;
			}

			Entity getLastEntity() const {
				return ecs.nextEntity - 1;
			}

			const ref<Body2D>& getBody(Entity entity) const {
				return ecs.getComponent<ref<Body2D>>(entity);
			}

			ref<Body2D>& getBody(Entity entity) {
				return ecs.getComponent<ref<Body2D>>(entity);
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

			Entity createDynamicEntity(uint32 layerIndex) {
				ENGINE_ASSERT(layerIndex < LAYER_COUNT, "The layer index is overflowing.");

				const Entity entity = ecs.createEntity();

				PhysicalObject2D physicalObject;

				ecs.setComponent<PhysicalObject2D>(entity, physicalObject);
				ecs.setComponent<DynamicEntityComponent>(entity, {});

				layers[layerIndex].emplace_back(entity);

				return entity;
			}

			// TODO: Difference between static/dynamic entities.
			Entity createStaticEntity(uint32 layerIndex) {
				ENGINE_ASSERT(layerIndex < LAYER_COUNT, "The layer index is overflowing.");

				const Entity entity = ecs.createEntity();

				PhysicalObject2D physicalObject;

				ecs.setComponent<PhysicalObject2D>(entity, physicalObject);
				ecs.setComponent<StaticEntityComponent>(entity, {});

				layers[layerIndex].emplace_back(entity);

				return entity;
			}

			void setBody(Entity entity, const ref<Body2D>& body) {
				ecs.setComponent<ref<Body2D>>(entity, body);
			}

			void setGeometry(Entity entity, const ref<Geometry2D>& geometry) {
				ecs.setComponent<ref<Geometry2D>>(entity, geometry);
			}

			void setMaterial(Entity entity, const ref<Material>& material) {
				ecs.setComponent<ref<Material>>(entity, material);
			}

			void updateMass(Entity entity) {
				const ref<Geometry2D>& geometry = ecs.getComponent<ref<Geometry2D>>(entity);
				const ref<Material>& material = ecs.getComponent<ref<Material>>(entity);

				ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(ecs.getComponent<ref<Body2D>>(entity));
				rigidBody->inverseMass = geometry->calculateMass(material->density);

				if (rigidBody->inverseMass != 0.0f) {
					rigidBody->inverseMass = 1.0f / rigidBody->inverseMass;
				}
			}

			void updateTransform(Entity entity) {
				const ref<RigidBody2D>& rigidBody = std::static_pointer_cast<RigidBody2D>(ecs.getComponent<ref<Body2D>>(entity));
				const ref<Geometry2D>& geometry = ecs.getComponent<ref<Geometry2D>>(entity);
				PhysicalObject2D& physicalObject = ecs.getComponent<PhysicalObject2D>(entity);

				physicalObject.transform =
					mat3x3<float32>::translation(-geometry->centerOfMass) *
					mat3x3<float32>::scaling(rigidBody->scale) *
					mat3x3<float32>::rotation(rigidBody->rotation) *
					mat3x3<float32>::translation(rigidBody->position);
			}

			/* const ComponentPool<DynamicEntityComponent>& pool = ecs.getComponentPool<DynamicEntityComponent>();

			for (uint32 componentIndex = 0; componentIndex < pool.componentCount; componentIndex += 1) {
				const Entity entity = pool.componentIndexToEntity[componentIndex];

				// TODO
			} */

			/*
			for (entity in dynamic entities) {
				for (other entity in dynamic entities) {
					//
				}
			}

			for (group index = 0; group index < group count - 1; group index += 1) {
				group = groups[group index]

				for (entity A in group) {
					for (other group index = group index + 1; other group index < group count; other group index += 1) {
						other group = groups[other group index]

						for (entity B in other group) {
							toi = time of impact between A and B

							if (toi < 1) {
								entity impulse = impulse between A and B
								group impulse = min(group impulse, entity impulse)
							}
						}
					}
				}

				apply group impulse
			}
			*/

		private:
			ECS ecs;
			std::array<std::vector<Entity>, LAYER_COUNT> layers;
	};
}