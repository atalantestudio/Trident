#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Physics/RigidBody.hpp"
#include "Engine/ECS/ECS.hpp"

namespace ProjectA {
	struct EntityDescriptor {
		vec2<float32> position = {0.0f, 0.0f};
		float32 rotation = 0.0f;
		vec2<float32> scale = {1.0f, 1.0f};
		float32 mass = 0.0f;
		float32 friction = 0.0f;
		float32 restitution = 0.0f;
		ref<Geometry2D> geometry;
		vec4<float32> fillColor = {1.0f, 1.0f, 1.0f, 1.0f};
		vec4<float32> strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
		uint32 layerIndex = 0;
	};

	struct DynamicEntityTag {};
	struct StaticEntityTag {};

	// TODO: Create "Runtime" directory for this kind of object?
	class Scene2D {
		private:
			static constexpr uint32 LAYER_COUNT = 4;

		public:
			explicit Scene2D() {
				// An entity of a 2D scene is composed of:
				// - 0-1 RigidBody2D
				// - 0-1 Geometry2D subclass
				// - 0-1 Material (base or subclass)
				// - 0-1 DynamicEntityTag to indicate that the entity is affected by forces.
				// - 0-1 StaticEntityTag to indicate that the entity is a fixed decoration.
				// 
				// DynamicEntityTag and StaticEntityTag are used by the physics engine to optimize broad and narrow phases.
				// They are not cumulable.

				ecs.registerComponent<RigidBody2D>();
				ecs.registerComponent<ref<Geometry2D>>();
				ecs.registerComponent<ref<Material>>();
				ecs.registerComponent<DynamicEntityTag>();
				ecs.registerComponent<StaticEntityTag>();
			}

			const std::array<std::vector<Entity>, LAYER_COUNT>& getLayers() const {
				return layers;
			}

			Entity getLastEntity() const {
				return ecs.nextEntity - 1;
			}

			const RigidBody2D& getRigidBody(Entity entity) const {
				return ecs.getComponent<RigidBody2D>(entity);
			}

			template<typename ComponentType>
			const ComponentPool<ComponentType>& getComponentPool() const {
				return ecs.getComponentPool<ComponentType>();
			}

			RigidBody2D& getRigidBody(Entity entity) {
				return ecs.getComponent<RigidBody2D>(entity);
			}

			const ref<Geometry2D>& getGeometry(Entity entity) const {
				return ecs.getComponent<ref<Geometry2D>>(entity);
			}

			const ref<Material>& getMaterial(Entity entity) const {
				return ecs.getComponent<ref<Material>>(entity);
			}

			/* const PhysicalObject2D& getPhysicalObject(Entity entity) const {
				return ecs.getComponent<PhysicalObject2D>(entity);
			} */

			void createDynamicEntity(const EntityDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);

				ecs.setComponent<DynamicEntityTag>(entity, {});
			}

			void createStaticEntity(const EntityDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);

				ecs.setComponent<StaticEntityTag>(entity, {});
			}

			void setGeometry(Entity entity, const ref<Geometry2D>& geometry) {
				ecs.setComponent<ref<Geometry2D>>(entity, geometry);
			}

			void setMaterial(Entity entity, const ref<Material>& material) {
				ecs.setComponent<ref<Material>>(entity, material);
			}

			const ComponentPool<DynamicEntityTag>& getDynamicEntityPool() const {
				return ecs.getComponentPool<DynamicEntityTag>();
			}

			const ComponentPool<StaticEntityTag>& getStaticEntityPool() const {
				return ecs.getComponentPool<StaticEntityTag>();
			}

		private:
			void setRigidBody(Entity entity, const RigidBody2D& body) {
				ecs.setComponent<RigidBody2D>(entity, body);
			}

			Entity createEntity(const EntityDescriptor& descriptor) {
				// TODO: Restore physical layers.
				// ENGINE_ASSERT(descriptor.layerIndex < LAYER_COUNT, "The layer index is overflowing.");

				const Entity entity = ecs.createEntity();

				setGeometry(entity, descriptor.geometry);

				RigidBody2D rigidBody{};
				rigidBody.position = descriptor.position;
				rigidBody.rotation = descriptor.rotation;
				rigidBody.scale = descriptor.scale;

				if (descriptor.mass == 0.0f) {
					rigidBody.inverseMass = 0.0f;
					rigidBody.inverseInertia = 0.0f;
				}
				else {
					rigidBody.inverseMass = 1.0f / descriptor.mass;

					const float32 A = area(descriptor.geometry->vertices) * 2.0f;

					// https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47
					rigidBody.inverseInertia = 1.0f / (descriptor.mass * A / 12.0f);
				}

				rigidBody.centerOfMass = descriptor.geometry->centroid;
				rigidBody.friction = descriptor.friction;
				rigidBody.geometry = ecs.getComponent<ref<Geometry2D>>(entity);
				rigidBody.updateTransform();

				setRigidBody(entity, rigidBody);

				ref<ColorMaterial> material = createRef<ColorMaterial>();
				material->fillColor = descriptor.fillColor;
				material->strokeColor = descriptor.strokeColor;

				setMaterial(entity, std::static_pointer_cast<Material>(material));

				// TODO: Restore physical layers.
				// layers[descriptor.layerIndex].emplace_back(entity);

				return entity;
			}

		private:
			ECS ecs;
			std::array<std::vector<Entity>, LAYER_COUNT> layers;
	};
}