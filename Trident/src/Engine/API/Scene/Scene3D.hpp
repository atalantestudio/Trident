// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/RigidBody3D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	struct InstanceDescriptor {
		uint32 geometryIndex;
		vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};
		vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		vec3<float32> scale = {1.0f, 1.0f, 1.0f};
	};

	class Scene3D : public Scene {
		private:
			static constexpr uint32 MAX_GEOMETRY_COUNT = 4;
			static constexpr uint32 MAX_INSTANCE_COUNT = 4;

		public:
			explicit Scene3D() :
				geometries(MAX_GEOMETRY_COUNT),
				instances(MAX_INSTANCE_COUNT)
			{}

			const sequence<RigidBody3D>& getInstances() const {
				return instances;
			}

			uint32 getInstanceCount() const {
				return instanceCount;
			}

			RigidBody3D& getInstance(uint32 instanceIndex) {
				DEBUG_ASSERT(instanceIndex < instanceCount);

				return instances[instanceIndex];
			}

			const RigidBody3D& getInstance(uint32 instanceIndex) const {
				DEBUG_ASSERT(instanceIndex < instanceCount);

				return instances[instanceIndex];
			}

			const sequence<ref<Geometry3D>>& getGeometries() const {
				return geometries;
			}

			uint32 getGeometryCount() const {
				return geometryCount;
			}

			ref<Geometry3D> getGeometry(uint32 geometryIndex) const {
				DEBUG_ASSERT(geometryIndex < geometryCount);

				return geometries[geometryIndex];
			}

			// Registers the provided geometry and returns its index in the geometry array.
			uint32 registerGeometry(ref<Geometry3D> geometry) {
				DEBUG_ASSERT(geometryCount < MAX_GEOMETRY_COUNT);

				const uint32 geometryIndex = geometryCount;

				geometries[geometryIndex] = geometry;

				geometryCount += 1;

				return geometryIndex;
			}

			// Registers the provided instance and returns its index in the instance array.
			uint32 registerInstance(const InstanceDescriptor& descriptor) {
				DEBUG_ASSERT(descriptor.geometryIndex < geometryCount);
				DEBUG_ASSERT(instanceCount < MAX_INSTANCE_COUNT);

				const uint32 instanceIndex = instanceCount;

				RigidBody3D& rigidBody = instances[instanceIndex];
				rigidBody.position = descriptor.position;
				rigidBody.rotation = descriptor.rotation;
				rigidBody.scale = descriptor.scale;
				rigidBody.geometryIndex = descriptor.geometryIndex;

				rigidBody.updateTransform();

				instanceCount += 1;

				return instanceIndex;
			}

			// Returns the number of instances (rigid bodies) using the provided geometry.
			uint32 getInstanceCount(uint32 geometryIndex) const {
				uint32 count = 0;

				for (uint32 instanceIndex = 0; instanceIndex < instanceCount; instanceIndex += 1) {
					const RigidBody3D& rigidBody = instances[instanceIndex];

					if (rigidBody.geometryIndex == geometryIndex) {
						count += 1;
					}
				}

				return count;
			}

		private:
			sequence<RigidBody3D> instances;
			sequence<ref<Geometry3D>> geometries;

			uint32 instanceCount = 0;
			uint32 geometryCount = 0;
	};
}