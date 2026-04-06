// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"

namespace Trident {
	class Scene3D : public Scene {
		public:
			explicit Scene3D(ref<Scene3DAsset> sceneAsset);

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

			void createDataBuffer(uint64 minStorageBufferOffsetAlignment);

		private:
			void loadGeometries(ref<Scene3DAsset> sceneAsset);
			void loadEntityComponentMappings(ref<Scene3DAsset> sceneAsset);

		private:
			uint64 vertexPositionComponentBufferSize;
			uint64 vertexPositionIndexBufferSize;
			uint64 vertexNormalComponentBufferSize;
			uint64 vertexNormalIndexBufferSize;
			uint64 vertexTextureCoordinateComponentBufferSize;
			uint64 vertexTextureCoordinateIndexBufferSize;
			uint64 vertexTangentComponentBufferSize;

			sequence<Geometry3D> geometries;

			std::vector<Shader> materialShaders;
			std::vector<std::vector<Material>> materials;
			std::vector<uint32> materialIndexToShaderIndex;
			uint32 currentMaterialIndex = 0;
	};
}