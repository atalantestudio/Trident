#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Material/ColorMaterial.hpp"
#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/Body/RigidBody.hpp"
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

	struct DynamicEntityComponent {};
	struct StaticEntityComponent {};

	// TODO: Create "Runtime" directory for this kind of object?
	class Scene2D {
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

			void createDynamicEntity(const EntityDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);

				ecs.setComponent<DynamicEntityComponent>(entity, {});
			}

			void createStaticEntity(const EntityDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);

				ecs.setComponent<StaticEntityComponent>(entity, {});
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

				const float32 m = geometry->calculateMass(material->density);

				// TODO: INFINITY constant?
				if (m == max<float32>()) {
					rigidBody->inverseMass = 0.0_kg;
					rigidBody->inverseInertia = 0.0f;
				
					return;
				}

				// https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47
				const float32 I = m * area(geometry->vertices) / 12.0f;

				rigidBody->inverseMass = rcp(m);
				rigidBody->inverseInertia = rcp(I);
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

			const ComponentPool<DynamicEntityComponent>& getDynamicEntityPool() const {
				return ecs.getComponentPool<DynamicEntityComponent>();
			}

			const ComponentPool<StaticEntityComponent>& getStaticEntityPool() const {
				return ecs.getComponentPool<StaticEntityComponent>();
			}

		private:
			Entity createEntity(const EntityDescriptor& descriptor) {
				// TODO: Restore physical layers.
				// ENGINE_ASSERT(descriptor.layerIndex < LAYER_COUNT, "The layer index is overflowing.");

				const Entity entity = ecs.createEntity();

				ref<RigidBody2D> rigidBody = createRef<RigidBody2D>();
				rigidBody->position = descriptor.position;
				rigidBody->rotation = descriptor.rotation;
				rigidBody->scale = descriptor.scale;

				if (descriptor.mass == 0.0f) {
					rigidBody->inverseMass = 0.0f;
					rigidBody->inverseInertia = 0.0f;
				}
				else {
					rigidBody->inverseMass = 1.0f / descriptor.mass;

					const float32 A = area(descriptor.geometry->vertices) * 2.0f;

					// https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47
					rigidBody->inverseInertia = 1.0f / (descriptor.mass * A / 12.0f);
				}

				rigidBody->friction = descriptor.friction;

				ref<ColorMaterial> material = createRef<ColorMaterial>();
				material->fillColor = descriptor.fillColor;
				material->strokeColor = descriptor.strokeColor;

				ecs.setComponent<PhysicalObject2D>(entity, {});

				setBody(entity, std::static_pointer_cast<Body2D>(rigidBody));
				setGeometry(entity, descriptor.geometry);
				setMaterial(entity, std::static_pointer_cast<Material>(material));
				// updateMass(entity);
				updateTransform(entity);

				// TODO: Restore physical layers.
				// layers[descriptor.layerIndex].emplace_back(entity);

				return entity;
			}

		private:
			ECS ecs;
			std::array<std::vector<Entity>, LAYER_COUNT> layers;
	};
}