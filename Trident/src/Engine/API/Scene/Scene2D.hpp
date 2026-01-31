// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/ColorFillStrokeMaterial.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	struct Entity2DDescriptor {
		vec2<float32> position = {0.0_m, 0.0_m};
		float32 rotation = 0.0_rad;
		vec2<float32> scale = {1.0f, 1.0f};
		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;
		float32 mass = 0.0_kg;
		float32 friction = 0.0f;
		float32 restitution = 0.0f;
		ref<Geometry2D> geometry;
		float32 density = 1.0f;
		vec4<float32> fillColor = {1.0f, 1.0f, 1.0f, 1.0f};
		vec4<float32> strokeColor = {0.0f, 0.0f, 0.0f, 1.0f};
		uint32 layerIndex = 0;
	};

	struct DynamicEntityTag {};
	struct StaticEntityTag {};

	class Scene2D : public Scene {
		private:
			static constexpr uint32 LAYER_COUNT = 4;

		public:
			explicit Scene2D() {
				ecs.registerComponent<RigidBody2D>();
				ecs.registerComponent<ref<Geometry2D>>();
				ecs.registerComponent<ref<PhysicsMaterial>>();
				ecs.registerComponent<DynamicEntityTag>();
				ecs.registerComponent<StaticEntityTag>();
			}

			const std::array<std::vector<Entity>, LAYER_COUNT>& getLayers() const {
				return layers;
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

			void createDynamicEntity(const Entity2DDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);
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

					const float32 area = descriptor.geometry->calculateMass(material->density, rigidBody.scale) * 2.0f;

					// TODO: Is this correct? See https://github.com/erincatto/box2d-lite/blob/227b71b6974ea57ab7e96d40f6374287bd6a0e77/src/Body.cpp#L47.
					const float32 I = m * area / 12.0f;

					rigidBody.inverseInertia = 1.0f / I;
				}

				ecs.setComponent<DynamicEntityTag>(entity, {});
			}

			void createStaticEntity(const Entity2DDescriptor& descriptor) {
				const Entity entity = createEntity(descriptor);

				RigidBody2D& rigidBody = getRigidBody(entity);
				rigidBody.inverseMass = 0.0_kg;
				rigidBody.inverseInertia = 0.0_kg_m2;

				ecs.setComponent<StaticEntityTag>(entity, {});
			}

			void setGeometry(Entity entity, ref<Geometry2D> geometry) {
				ecs.setComponent<ref<Geometry2D>>(entity, geometry);
			}

			void setMaterial(Entity entity, ref<PhysicsMaterial> material) {
				ecs.setComponent<ref<PhysicsMaterial>>(entity, material);
			}

		private:
			Entity createEntity(const Entity2DDescriptor& descriptor) {
				// TODO: Restore physical layers.
				// ENGINE_ASSERT(descriptor.layerIndex < LAYER_COUNT, "The layer index is overflowing.");

				const Entity entity = ecs.createEntity();

				setGeometry(entity, descriptor.geometry);

				// TODO: Set correct restitution.
				ref<ColorFillStrokeMaterial> material = createRef<ColorFillStrokeMaterial>();
				material->density = descriptor.density;
				material->fillColor = descriptor.fillColor;
				material->strokeColor = descriptor.strokeColor;

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

				rigidBody.updateTransform();

				ecs.setComponent<RigidBody2D>(entity, rigidBody);

				// TODO: Restore physical layers.
				// layers[descriptor.layerIndex].emplace_back(entity);

				return entity;
			}

		private:
			std::array<std::vector<Entity>, LAYER_COUNT> layers;
	};
}