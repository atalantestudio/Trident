// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry3D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"

namespace Trident {
	class Scene3D : public Scene {
		public:
			explicit Scene3D(ref<Scene3DAsset> sceneAsset, uint64 minStorageBufferOffsetAlignment);

			inline range<uint64, uint64> getVertexPositionComponentRange() const {
				return vertexPositionComponentRange;
			}

			inline range<uint64, uint64> getVertexNormalComponentRange() const {
				return vertexNormalComponentRange;
			}

			inline range<uint64, uint64> getVertexTextureCoordinateComponentRange() const {
				return vertexTextureCoordinateComponentRange;
			}

			inline range<uint64, uint64> getTriangleTangentComponentRange() const {
				return triangleTangentComponentRange;
			}

			inline range<uint64, uint64> getVertexPositionIndexRange() const {
				return vertexPositionIndexRange;
			}

			inline range<uint64, uint64> getVertexNormalIndexRange() const {
				return vertexNormalIndexRange;
			}

			inline range<uint64, uint64> getVertexTextureCoordinateIndexRange() const {
				return vertexTextureCoordinateIndexRange;
			}

			inline const sequence<Geometry3D>& getGeometries() const {
				return geometries;
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

		private:
			void loadGeometries(ref<Scene3DAsset> sceneAsset, uint64 minStorageBufferOffsetAlignment);
			void loadEntityComponentMappings(ref<Scene3DAsset> sceneAsset);

		private:
			/// Ranges in bytes of the descriptor buffers, aligned to the `minStorageBufferOffsetAlignment` limit.
			range<uint64, uint64> vertexPositionComponentRange;
			range<uint64, uint64> vertexNormalComponentRange;
			range<uint64, uint64> vertexTextureCoordinateComponentRange;
			range<uint64, uint64> triangleTangentComponentRange;
			range<uint64, uint64> vertexPositionIndexRange;
			range<uint64, uint64> vertexNormalIndexRange;
			range<uint64, uint64> vertexTextureCoordinateIndexRange;

			sequence<Geometry3D> geometries;

			// TODO: Implement 3D hulls.

			std::vector<Shader> materialShaders;
			std::vector<std::vector<Material>> materials;
			std::vector<uint32> materialIndexToShaderIndex;
			uint32 currentMaterialIndex = 0;
	};
}