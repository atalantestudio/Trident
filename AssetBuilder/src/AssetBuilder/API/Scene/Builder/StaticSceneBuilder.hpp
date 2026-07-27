// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "AssetBuilder/API/Scene/StaticSurface.hpp"

#include "Engine/API/Scene/StaticVertex.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct SceneBuilderDescriptor {
		svec3<float32> skyColor;
		svec3<float32> sunDirection;
		svec3<float32> sunColor;
		float32 sunIntensity;
	};

	class StaticSceneBuilder {
		public:
			static Lightmapper lightmapper;

		private:
			static constexpr Extent2D LIGHTMAP_ATLAS_EXTENT = {1024, 1024};

		public:
			static StaticGeometry3D createWallGeometry(svec2<float32> scale);

		private:
			static LightmapNode* insertLightmapNode(Lightmap& lightmap, LightmapNode* rootNode);

		public:
			explicit StaticSceneBuilder(const SceneBuilderDescriptor& descriptor);

			void addMaterialShader(sequence<char8> name, ShaderStage stage, sequence<char8> entryPoint, sequence<DescriptorSetLayoutBinding> bindings);

			uint32 addMaterial(ref<MaterialAsset> materialAsset);

			uint32 placeSurface(const SurfaceDescriptor& descriptor, const Transform& transform);

			uint32 snapSurface(const SurfaceDescriptor& descriptor, const StaticEdge& parentEdge, uint32 edgeIndex, float32 hingeAngle);
			uint32 snapSurface(const DualSnapSurfaceDescriptor& descriptor, const DualSnapEdge& edgeA, const DualSnapEdge& edgeB);

			/// Compiles and returns an optimized static scene.
			StaticScene3D build();

		private:
			uint32 compileSurface(const SurfaceDescriptor& descriptor, const Transform& transform, vec3<float32> hingeOffset, quat<float32> hingeRotation, bool textureFilteringTransform);

			void carveSurface(StaticSurface& surface, svec3<float32> origin, const std::vector<Carving>& carvings, uint32& surfaceVertexCount) const;

			void generateLightmaps(StaticScene3D& scene);

			void createLightmaps(StaticScene3D& scene);

			void updateLightmapCoordinates(StaticScene3D& scene);

		protected:
			svec3<float32> skyColor;
			svec3<float32> sunDirection;
			svec3<float32> sunColor;
			float32 sunIntensity;

			std::vector<StaticSurface> surfaces;

			std::vector<std::vector<StaticVertex3D>> indicesPerMaterial;
			std::vector<svec3<float32>> vertices;
			std::vector<svec3<float32>> normals;
			std::vector<svec2<float32>> textureCoordinates;
			std::vector<BuildMaterialShader> materialShaders;
			std::vector<BuildStaticMaterial3D> materials;

			std::vector<Lightmap> lightmaps;
			sequence<svec2<float32>> lightmapCoordinates;

			/// Sorted by ascending area.
			std::vector<LightmapLookup> lightmapLookups;

			uint32 indexCount = 0;
			uint32 subsurfaceCount = 0;
			uint32 subsurfaceTextureCoordinateCount = 0;
	};
}