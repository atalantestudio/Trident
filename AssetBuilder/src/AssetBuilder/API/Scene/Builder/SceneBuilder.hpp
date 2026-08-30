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

#include "Engine/API/Light/PointLight.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	class SceneBuilder {
		private:
			static constexpr Extent2D LIGHTMAP_ATLAS_EXTENT = {1024, 1024};

			static Lightmapper lightmapper;

		public:
			static BuildStaticGeometry3D createWallGeometry(svec2<float32> scale);

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
			explicit SceneBuilder(const Scene3DDescriptor& descriptor);

			/// Loads a geometry asset and returns its index.
			uint32 loadGeometry(ref<Geometry3DAsset> asset);

			MaterialReference loadMaterial(ref<MaterialAsset> asset);

			void placeSurface(const SurfaceDescriptor& descriptor, const TransformAngleAxis3D& transform);

			void placeStaticProp(const MeshDescriptor& descriptor);

			void placeRigidBody(const EntityDescriptor& descriptor);

			void placePointLight(const PointLight& pointLight);

			BuildScene3D build();

		private:
			uint32 findMaterialShaderIndex(uint32 assetIdentifier) const;
			uint32 findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex) const;

			void buildBakedGeometry(BuildScene3D& scene);
			void buildEntityComponentMappings(BuildDynamicSceneGeometry3D& dynamicGeometry);
			void buildLights(BuildDynamicSceneGeometry3D& dynamicGeometry);
			void buildLightmaps(BuildScene3D& scene);

			void createLightmaps(BuildStaticSceneGeometry3D& staticGeometry);
			void updateLightmapCoordinates(BuildStaticSceneGeometry3D& staticGeometry);
			void createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<BuildVertex3D>& vertices, sequence<uint32>& indices, sequence<LightmapMaterial>& materials, sequence<uint32>& emissiveTriangleIndices);

		protected:
			uint64 bakedVertexCount = 0;
			uint64 bakedIndexCount = 0;
			uint64 staticVertexCount = 0;
			uint64 staticIndexCount = 0;
			uint64 staticInstanceCount = 0;
			uint64 dynamicVertexCount = 0;
			uint64 dynamicIndexCount = 0;
			uint64 lightmapVertexCount = 0;
			uint64 lightmapIndexCount = 0;
			uint64 materialGeometryCount = 0;
			uint64 materialCount = 0;
			uint32 entityCount = 0;

			std::vector<IntermediateGeometry> geometries;
			std::vector<IntermediateMaterialShader> materialShaders;

			std::vector<Lightmap> lightmaps;
			std::vector<LightmapMaterial> lightmapMaterials;

			/// Sorted by ascending area.
			std::vector<LightmapLookup> lightmapLookups;

			Scene3DDescriptor descriptor;

			std::vector<EntityComponentMapping<GeometryComponent>> geometryMappings;
			std::vector<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;

			std::vector<PointLight> pointLights;
	};
}