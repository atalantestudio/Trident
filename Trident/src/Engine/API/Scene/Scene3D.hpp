// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialReference.hpp"
#include "Engine/API/Physics/RigidBody3D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	struct SceneInstanceDescriptor {
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
				vertexPositionIndexOffsets(MAX_GEOMETRY_COUNT),
				materials(MAX_MATERIAL_COUNT),
				materialReferences(MAX_MATERIAL_COUNT),
				flatMaterialReferences(MAX_MATERIAL_REFERENCE_COUNT),
				instances(MAX_INSTANCE_COUNT)
			{}

			inline const sequence<uint8>& getVertexBuffer() const {
				return vertexBuffer;
			}

			inline uint64 getVertexPositionComponentBufferSize() const {
				return vertexPositionComponentBufferSize;
			}

			inline uint64 getVertexPositionIndexBufferSize() const {
				return vertexPositionIndexBufferSize;
			}

			inline uint64 getVertexNormalComponentBufferSize() const {
				return vertexNormalComponentBufferSize;
			}

			inline uint64 getVertexNormalIndexBufferSize() const {
				return vertexNormalIndexBufferSize;
			}

			inline uint64 getVertexTextureCoordinateComponentBufferSize() const {
				return vertexTextureCoordinateComponentBufferSize;
			}

			inline uint64 getVertexTextureCoordinateIndexBufferSize() const {
				return vertexTextureCoordinateIndexBufferSize;
			}

			inline uint64 getVertexTangentComponentBufferSize() const {
				return vertexTangentComponentBufferSize;
			}


			inline view<ref<Geometry3D>> getGeometries() const {
				return view<ref<Geometry3D>>(&geometries[0], geometryCount);
			}

			inline uint32 getGeometryCount() const {
				return geometryCount;
			}

			inline uint32 getVertexPositionIndexOffset(uint32 geometryIndex) const {
				DEBUG_ASSERT(geometryIndex < geometryCount);

				return vertexPositionIndexOffsets[geometryIndex];
			}

			inline view<Material> getMaterials() const {
				return view<Material>(&materials[0], materialCount);
			}

			inline uint32 getMaterialCount() const {
				return materialCount;
			}

			inline uint32 getMaterialReferenceCount() const {
				return materialReferenceCount;
			}

			inline const sequence<std::vector<MaterialReference>>& getAllMaterialReferences() const {
				return materialReferences;
			}

			inline const sequence<MaterialReference>& getFlatMaterialReferences() const {
				return flatMaterialReferences;
			}

			inline view<MaterialReference> getMaterialReferences(uint32 materialIndex) const {
				const std::vector<MaterialReference>& references = materialReferences[materialIndex];

				if (references.size() == 0) {
					return {};
				}

				return view<MaterialReference>(&references[0], references.size());
			}

			inline view<RigidBody3D> getInstances() const {
				return view<RigidBody3D>(&instances[0], instanceCount);
			}

			inline uint32 getInstanceCount() const {
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

			uint32 registerInstance(const SceneInstanceDescriptor& descriptor) {
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

				vertexPositionIndexOffsets[geometryIndex] = vertexPositionIndexOffset;
				vertexPositionIndexOffset += static_cast<uint32>(geometry->getVertexPositionIndices().count());

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
				DEBUG_ASSERT(materialReference.materialIndex < MAX_MATERIAL_COUNT);

				const uint32 materialReferenceIndex = materialReferenceCount;

				materialReferences[materialReference.materialIndex].emplace_back(materialReference);
				flatMaterialReferences[materialReferenceCount] = materialReference;
				materialReferenceCount += 1;

				return materialReferenceIndex;
			}

			void createVertexBuffer(uint64 minStorageBufferOffsetAlignment);

		private:
			// Contains components and indices for vertex positions, texture coordinates and normals.
			sequence<uint8> vertexBuffer;

			uint64 vertexPositionComponentBufferSize;
			uint64 vertexPositionIndexBufferSize;
			uint64 vertexNormalComponentBufferSize;
			uint64 vertexNormalIndexBufferSize;
			uint64 vertexTextureCoordinateComponentBufferSize;
			uint64 vertexTextureCoordinateIndexBufferSize;
			uint64 vertexTangentComponentBufferSize;

			sequence<ref<Geometry3D>> geometries;
			sequence<uint32> vertexPositionIndexOffsets;
			sequence<Material> materials;
			sequence<std::vector<MaterialReference>> materialReferences;
			sequence<MaterialReference> flatMaterialReferences;
			sequence<RigidBody3D> instances;
			uint32 geometryCount = 0;
			uint32 vertexPositionIndexOffset = 0;
			uint32 materialCount = 0;
			uint32 materialReferenceCount = 0;
			uint32 instanceCount = 0;
	};
}