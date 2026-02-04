// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialReference.hpp"
#include "Engine/API/Physics/RigidBody3D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	struct InstanceDescriptor {
		vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};
		vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		vec3<float32> scale = {1.0f, 1.0f, 1.0f};
		uint32 geometryIndex;
		uint32 materialReferenceIndex;
	};

	class Scene3D : public Scene {
		private:
			static constexpr uint32 MAX_GEOMETRY_COUNT = 4;
			static constexpr uint32 MAX_MATERIAL_COUNT = 16;
			static constexpr uint32 MAX_MATERIAL_REFERENCE_COUNT = 32;
			static constexpr uint32 MAX_INSTANCE_COUNT = 4;

		public:
			explicit Scene3D() :
				geometries(MAX_GEOMETRY_COUNT),
				materials(MAX_MATERIAL_COUNT),
				materialReferences(MAX_MATERIAL_REFERENCE_COUNT),
				instances(MAX_INSTANCE_COUNT)
			{}

			view<ref<Geometry3D>> getGeometries() const {
				return view<ref<Geometry3D>>(&geometries[0], geometryCount);
			}

			uint32 getGeometryCount() const {
				return geometryCount;
			}

			view<Material> getMaterials() const {
				return view<Material>(&materials[0], materialCount);
			}

			uint32 getMaterialCount() const {
				return materialCount;
			}

			view<MaterialReference> getMaterialReferences() const {
				return view<MaterialReference>(&materialReferences[0], materialReferenceCount);
			}

			uint32 getMaterialReferenceCount() const {
				return materialReferenceCount;
			}

			view<RigidBody3D> getInstances() const {
				return view<RigidBody3D>(&instances[0], instanceCount);
			}

			uint32 getInstanceCount() const {
				return instanceCount;
			}

			/// Returns the number of instances using the provided geometry.
			uint32 getInstanceCount(uint32 geometryIndex) const {
				uint32 geometryInstanceCount = 0;
				uint32 instanceIndex = 0;

				while (instanceIndex < instanceCount) {
					const RigidBody3D& rigidBody = instances[instanceIndex];

					if (rigidBody.geometryIndex == geometryIndex) {
						geometryInstanceCount += 1;
					}

					instanceIndex += 1;
				}

				return geometryInstanceCount;
			}

			uint32 registerInstance(const InstanceDescriptor& descriptor) {
				DEBUG_ASSERT(descriptor.geometryIndex < geometryCount);
				DEBUG_ASSERT(instanceCount < MAX_INSTANCE_COUNT);

				const uint32 instanceIndex = instanceCount;

				RigidBody3D& rigidBody = instances[instanceIndex];
				rigidBody.position = descriptor.position;
				rigidBody.rotation = descriptor.rotation;
				rigidBody.scale = descriptor.scale;
				rigidBody.geometryIndex = descriptor.geometryIndex;
				rigidBody.materialReferenceIndex = descriptor.materialReferenceIndex;

				rigidBody.updateTransform();

				instanceCount += 1;

				return instanceIndex;
			}

			uint32 registerGeometry(ref<Geometry3D> geometry) {
				DEBUG_ASSERT(geometryCount < MAX_GEOMETRY_COUNT);

				const uint32 geometryIndex = geometryCount;

				geometries[geometryIndex] = geometry;
				geometryCount += 1;

				return geometryIndex;
			}

			uint32 registerMaterial(const Material& material) {
				DEBUG_ASSERT(materialCount < MAX_MATERIAL_COUNT);

				const uint32 materialIndex = materialCount;

				materials[materialIndex] = material;
				materialCount += 1;

				return materialIndex;
			}

			uint32 registerMaterialReference(const MaterialReference& materialReference) {
				DEBUG_ASSERT(materialReferenceCount < MAX_MATERIAL_REFERENCE_COUNT);

				const uint32 materialReferenceIndex = materialReferenceCount;

				materialReferences[materialReferenceIndex] = materialReference;
				materialReferenceCount += 1;

				return materialReferenceIndex;
			}

		private:
			sequence<ref<Geometry3D>> geometries;
			sequence<Material> materials;
			sequence<MaterialReference> materialReferences;
			sequence<RigidBody3D> instances;
			uint32 geometryCount = 0;
			uint32 materialCount = 0;
			uint32 materialReferenceCount = 0;
			uint32 instanceCount = 0;
	};
}