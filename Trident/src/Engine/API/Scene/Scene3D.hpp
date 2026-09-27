// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/API/Scene/Scene.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/Scene3DAsset.hpp"

namespace Trident {
	class Scene3D : public Scene {
		public:
			explicit Scene3D(ref<Scene3DAsset> sceneAsset);

			inline ref<Scene3DAsset> getAsset() const {
				return asset;
			}

			inline uint32 getIndexCount() const {
				return indexCount;
			}

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

			inline range<uint64, uint64> getIndexRange() const {
				return indexRange;
			}

			inline uint64 getHullLineIndexByteOffset() const {
				return hullLineIndexByteOffset;
			}

			inline uint32 getMaterialShaderCount() const {
				return materialShaderCount;
			}

			inline const std::vector<std::vector<ref<MaterialAsset>>>& getMaterialAssets() const {
				return materialAssets;
			}

			inline uint32 getMaterialShaderIndex(uint32 materialAssetIndex) const {
				return materialAssetIndexToShaderIndex[materialAssetIndex];
			}

			/// Registers a new material shader and returns its index.
			uint32 registerMaterialShader() {
				const uint32 materialShaderIndex = materialShaderCount;

				materialShaderCount += 1;

				return materialShaderIndex;
			}

			/// Registers a new material and returns its index.
			uint32 registerMaterialAsset(ref<MaterialAsset> materialAsset, uint32 shaderIndex) {
				ENGINE_ASSERT(shaderIndex < materialShaderCount, "Material is using an unregistered shader (index []).", shaderIndex);

				const uint32 materialIndex = currentMaterialIndex;

				if (shaderIndex >= materialAssets.size()) {
					materialAssets.resize(shaderIndex + 1);
				}

				materialAssets[shaderIndex].emplace_back(materialAsset);
				materialAssetIndexToShaderIndex.emplace_back(shaderIndex);

				currentMaterialIndex += 1;

				return materialIndex;
			}

			void updateRigidBodyFacePlanes();

		private:
			void loadGeometries();
			void loadEntityComponentMappings();

		private:
			ref<Scene3DAsset> asset;

			/// Total number of indices.
			uint32 indexCount;

			range<uint64, uint64> vertexPositionComponentRange;
			range<uint64, uint64> vertexNormalComponentRange;
			range<uint64, uint64> vertexTextureCoordinateComponentRange;
			range<uint64, uint64> triangleTangentComponentRange;
			range<uint64, uint64> indexRange;

			uint64 hullLineIndexByteOffset;

		public:
			sequence<Edge3D> hullEdges;
			sequence<Plane3D> hullFaces;
			sequence<uint16> hullFaceEdgeIndices;
			sequence<Plane3D> rigidBodyFacePlanes;

		private:
			uint32 materialShaderCount = 0;
			std::vector<std::vector<ref<MaterialAsset>>> materialAssets;
			std::vector<uint32> materialAssetIndexToShaderIndex;
			uint32 currentMaterialIndex = 0;
	};
}