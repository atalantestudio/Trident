// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Light/PointLight.hpp"
#include "Engine/Filesystem/Asset/Geometry3DAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

#include "AssetBuilder/API/Entity/EntityDescriptor.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmap.hpp"
#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"
#include "AssetBuilder/API/Map/Face.hpp"
#include "AssetBuilder/API/Map/Intermediate.hpp"
#include "AssetBuilder/API/Map/Scene3D.hpp"
#include "AssetBuilder/API/Material/MaterialReference.hpp"
#include "AssetBuilder/API/Mesh/MeshDescriptor.hpp"

namespace Trident {
	class MapBuilder {
		public:
			static constexpr uint32 MAX_FACE_POSITIONS = 65535;
			static constexpr uint32 MAX_INDICES = 65535;
			static constexpr uint32 MAX_FACES = 256;
			static constexpr Extent2D LIGHTMAP_ATLAS_EXTENT = {1024, 1024};

		private:
			// TODO(asset-builder/scene): Remove once TSL is ready.
			static ShaderModuleAsset getMaterialShaderModuleAsset(uint32 identifier);

			/// See https://blackpawn.com/texts/lightmaps.
			static LightmapNode* insertLightmapNode(const Lightmap& lightmap, LightmapNode* node);

		public:
			explicit MapBuilder(const MapDescriptor& descriptor);

			/// Loads a geometry asset and returns its index.
			uint32 loadGeometry(ref<Geometry3DAsset> asset);

			MaterialReference loadMaterial(ref<MaterialAsset> asset);

			Face& getFace(uint32 index);

			/// Creates a new point to be referenced by faces.
			void createPoint(svec3<float32> position);

			/// Creates and returns a new face.
			/// The face must have at least 3 vertices laid out in clockwise order.
			/// Degenerate vertices and edge-crossing vertices are not allowed.
			Face& createFace(const FaceDescriptor& descriptor);
			Face& createFace(const IndexedFaceDescriptor& descriptor);

			/// Extrudes the face `scale` units towards its normal.
			/// This operation moves the current face and creates additional faces on the extruded sides.
			/// 
			/// If `scale` is 0, the operation is ignored.
			void extrudeFace(Face& face, float32 scale);

			/// Carves a new face inside an existing face.
			/// This operation modifies the vertices of the current face.
			Face& carveFace(Face& face, view<svec3<float32>> carvingVertices);

			void createStaticProp(const MeshDescriptor& descriptor);

			void createRigidBody(const EntityDescriptor& descriptor);

			void createPointLight(const PointLight& pointLight);

			BuildScene3D build(Lightmapper* lightmapper);

		private:
			uint32 findMaterialShaderIndex(uint32 assetIdentifier) const;
			uint32 findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex) const;

			// TODO(asset-builder/map/static-geometry/face-carving): Ensure that the carving vertices are all inside or on the edge of the face.
			/// Returns a pair of vertex indices where A is the closest face vertex to the carving
			/// and B is the closest carving index to the face.
			Edge2D findClosestFaceCarvingConnectorEdge(const Face& face, view<svec3<float32>> carvingVertices) const;

			void createFaceGeometries();

			void calculateFaceBoundingBoxes(Face& face) const;

			void triangulateFaces(IntermediateGeometry& geometry, uint32 lineIndexOffset);

			void buildEntityComponentMappings(BuildDynamicSceneGeometry3D& dynamicGeometry);
			void buildLights(BuildDynamicSceneGeometry3D& dynamicGeometry);
			void buildLightmaps(BuildScene3D& scene, Lightmapper* lightmapper);

			void createLightmaps(const BuildScene3D& scene, BuildStaticSceneGeometry3D& staticGeometry);
			void updateLightmapCoordinates(BuildStaticSceneGeometry3D& staticGeometry);

			sequence<BuildTriangle3D> createTriangleBuffer(const BuildScene3D& scene) const;
			sequence<uint32> createEmissiveTriangleIndexBuffer(const BuildScene3D& scene) const;

		protected:
			uint64 bakedVertexCount = 0;
			uint64 bakedIndexCount = 0;
			uint64 lightmapCoordinateCount = 0;
			uint64 staticInstanceCount = 0;
			uint64 dynamicVertexCount = 0;
			uint64 dynamicIndexCount = 0;
			uint64 materialGeometryCount = 0;
			uint64 materialCount = 0;
			uint64 lightmapCount = 0;
			uint32 entityCount = 0;

			sequence<svec3<float32>> facePositions;
			sequence<Face> faces;

			/// Total unique position count.
			uint32 facePositionCount = 0;

			uint32 staticGeometryFaceCount = 0;

			std::vector<IntermediateGeometry> geometries;
			std::vector<IntermediateMaterialShader> materialShaders;

			sequence<Lightmap> lightmaps;
			sequence<LightmapMaterial> lightmapMaterials;

			/// Sorted by ascending area.
			std::vector<LightmapLookup> lightmapLookups;

			MapDescriptor descriptor;

			std::vector<EntityComponentMapping<GeometryComponent>> geometryMappings;
			std::vector<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;

			std::vector<PointLight> pointLights;
	};
}