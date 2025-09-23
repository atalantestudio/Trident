#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Physics/RigidBody3D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace ProjectA {
	struct Entity3DDescriptor {
		vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};
		vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		vec3<float32> scale = {1.0f, 1.0f, 1.0f};
		ref<Geometry3D> geometry;
		ref<Material> material;
	};

	class Scene3D : public Scene {
		public:
			explicit Scene3D() {
				ecs.registerComponent<RigidBody3D>();
				ecs.registerComponent<ref<Geometry3D>>();
				ecs.registerComponent<ref<Material>>();
			}

			RigidBody3D& getRigidBody(Entity entity) {
				return ecs.getComponent<RigidBody3D>(entity);
			}

			const RigidBody3D& getRigidBody(Entity entity) const {
				return ecs.getComponent<RigidBody3D>(entity);
			}

			const ref<Geometry3D>& getGeometry(Entity entity) const {
				return ecs.getComponent<ref<Geometry3D>>(entity);
			}

			const ref<Material>& getMaterial(Entity entity) const {
				return ecs.getComponent<ref<Material>>(entity);
			}

			void createEntity(const Entity3DDescriptor& descriptor) {
				const Entity entity = ecs.createEntity();

				const uint32 geometryIndex = ecs.getComponentPool<ref<Geometry3D>>().componentCount;

				ecs.setComponent<ref<Geometry3D>>(entity, descriptor.geometry);
				ecs.setComponent<ref<Material>>(entity, std::static_pointer_cast<Material>(descriptor.material));

				RigidBody3D rigidBody{};
				rigidBody.position = descriptor.position;
				rigidBody.rotation = descriptor.rotation;
				rigidBody.scale = descriptor.scale;
				rigidBody.geometryIndex = geometryIndex;

				rigidBody.updateTransform();

				ecs.setComponent<RigidBody3D>(entity, rigidBody);
			}
	};
}