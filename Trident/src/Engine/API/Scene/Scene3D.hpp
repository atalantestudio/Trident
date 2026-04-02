// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Physics/RigidBody3D.hpp"
#include "Engine/API/Scene/Scene.hpp"

namespace Trident {
	struct SceneInstanceDescriptor {
		vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};
		vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		vec3<float32> scale = {1.0f, 1.0f, 1.0f};
		uint32 geometryIndex;
		uint32 materialIndex;
	};

	class Scene3D : public Scene {
		private:
			static constexpr uint32 MAX_GEOMETRY_COUNT = 4;
			static constexpr uint32 MAX_INSTANCE_COUNT = 4;

		public:
			explicit Scene3D() :
				geometries(MAX_GEOMETRY_COUNT),
				vertexPositionIndexOffsets(MAX_GEOMETRY_COUNT),
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

			inline const std::vector<Shader>& getMaterialShaders() const {
				return materialShaders;
			}

			inline const std::vector<std::vector<Material>>& getMaterials() const {
				return materials;
			}

			inline uint32 getMaterialShaderIndex(uint32 materialIndex) const {
				return materialIndexToShaderIndex[materialIndex];
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
				rigidBody.materialIndex = descriptor.materialIndex;

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

			/// Registers a new material shader and returns its index.
			uint32 registerMaterialShader(const Shader& materialShader) {
				const uint32 materialShaderIndex = static_cast<uint32>(materialShaders.size());

				materialShaders.emplace_back(materialShader);

				return materialShaderIndex;
			}

			/// Registers a new material and returns its index.
			uint32 registerMaterial(const Material& material) {
				ENGINE_ASSERT(material.shaderIndex < materialShaders.size(), "Material is using an unregistered shader (index []).", material.shaderIndex);

				const uint32 materialIndex = currentMaterialIndex;

				if (material.shaderIndex >= materials.size()) {
					materials.resize(material.shaderIndex + 1);
				}

				materials[material.shaderIndex].emplace_back(material);
				materialIndexToShaderIndex.emplace_back(material.shaderIndex);

				currentMaterialIndex += 1;

				return materialIndex;
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
			sequence<RigidBody3D> instances;
			uint32 geometryCount = 0;
			uint32 vertexPositionIndexOffset = 0;
			uint32 instanceCount = 0;
			uint32 currentMaterialIndex = 0;

			std::vector<Shader> materialShaders;
			std::vector<std::vector<Material>> materials;
			std::vector<uint32> materialIndexToShaderIndex;
	};
}