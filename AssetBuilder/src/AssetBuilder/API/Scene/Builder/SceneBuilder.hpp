// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Entity/EntityDescriptor.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"
#include "AssetBuilder/API/Mesh/MeshDescriptor.hpp"
#include "AssetBuilder/API/Scene/Builder/Intermediate.hpp"
#include "AssetBuilder/API/Scene/Scene3D.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	class SceneBuilder {
		private:
			static constexpr Extent2D LIGHTMAP_ATLAS_EXTENT = {1024, 1024};

			static Lightmapper lightmapper;

		public:
			static StaticGeometry3D createWallGeometry(svec2<float32> scale);

			static void release();

		private:
			template<typename T>
			static void resizeBuffer(sequence<T>& buffer, uint64 additionalCount) {
				sequence<T> resizedBuffer(buffer.count() + additionalCount);

				std::copy(buffer.begin(), buffer.end(), &resizedBuffer[0]);

				buffer = resizedBuffer;
			}

			// TODO(asset-builder/scene): Remove once TSL is ready.
			static ShaderModuleAsset getMaterialShaderModuleAsset(uint32 identifier);

			static LightmapNode* insertLightmapNode(Lightmap& lightmap, LightmapNode* node);

		public:
			explicit SceneBuilder(const LightmapperArguments& lightmapperArguments);

			/// Loads a geometry asset and returns its index.
			uint32 loadGeometry(ref<GeometryAsset> asset);

			MaterialReference loadStaticMaterial(ref<MaterialAsset> asset);
			MaterialReference loadDynamicMaterial(ref<MaterialAsset> asset);

			void placeSurface(const SurfaceDescriptor& descriptor, const TransformAngleAxis3D& transform);

			void placeStaticProp(const MeshDescriptor& descriptor);

			void placeRigidBody(const EntityDescriptor& descriptor);

			BuildScene3D build();

		private:
			IntermediateMaterial& getMaterial(MaterialReference materialReference);

			MaterialReference loadMaterial(ref<MaterialAsset> asset, std::vector<IntermediateMaterialShader>& materialShaders);

			uint32 findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex);

			void buildGeometries(BuildScene3D& scene);
			void buildMaterialGeometries(BuildScene3D& scene);
			void buildMaterials(BuildScene3D& scene);
			void buildMaterialShaders(BuildScene3D& scene);
			void buildBakedGeometry(BuildStaticSceneGeometry3D& staticGeometry);
			void buildLightmaps(BuildScene3D& scene);
			void buildEntityComponentMappings(BuildDynamicSceneGeometry3D& dynamicGeometry);

			void createLightmaps(BuildScene3D& scene);
			void updateLightmapCoordinates(BuildStaticSceneGeometry3D& staticGeometry);
			void createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<GeometryIndex3D>& vertices, sequence<svec3<float32>>& positions, sequence<svec3<float32>>& normals);

		protected:
			uint64 bakedVertexCount = 0;
			uint64 bakedPositionCount = 0;
			uint64 bakedNormalCount = 0;
			uint64 bakedTextureCoordinateCount = 0;

			uint64 staticVertexCount = 0;
			uint64 staticPositionCount = 0;
			uint64 staticNormalCount = 0;
			uint64 staticTextureCoordinateCount = 0;
			uint64 staticInstanceCount = 0;

			uint64 dynamicVertexCount = 0;
			uint64 dynamicPositionCount = 0;
			uint64 dynamicNormalCount = 0;
			uint64 dynamicTextureCoordinateCount = 0;

			uint64 lightmapCoordinateCount = 0;

			uint64 materialGeometryCount = 0;
			uint64 materialCount = 0;
			uint32 entityCount = 0;

			std::vector<IntermediateGeometry> geometries;
			std::vector<IntermediateMaterialShader> staticMaterialShaders;
			std::vector<IntermediateMaterialShader> dynamicMaterialShaders;

			std::vector<Lightmap> lightmaps;

			/// Sorted by ascending area.
			std::vector<LightmapLookup> lightmapLookups;

			LightmapperArguments lightmapperArguments;

			std::vector<EntityComponentMapping<GeometryComponent>> geometryMappings;
			std::vector<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};
}