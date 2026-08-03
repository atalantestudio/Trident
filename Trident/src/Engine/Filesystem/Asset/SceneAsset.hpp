// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialShader.hpp"
#include "Engine/API/Material/StaticMaterial.hpp"
#include "Engine/API/Scene/StaticVertex.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct SceneAsset;
	struct Scene2DAsset;
	struct Scene3DAsset;

	struct SceneAssetHeader : public AssetHeader {
		uint32 staticTriangleCount;
		uint32 staticVertexCount;
		uint32 staticTextureCoordinateCount;
		uint32 entityCount;
		uint32 geometryAssetCount;
		uint32 geometryMappingCount;
		uint32 materialMappingCount;
		uint32 rigidBodyMappingCount;
	};

	template<>
	struct Descriptor<SceneAsset> : public Asset {
		sequence<uint8> staticGeometryBuffer;
		uint64 dynamicVertexPositionCount;
		uint64 dynamicVertexTextureCoordinateCount;

		// NOTE
		// In 2D, this contains the line indices of the hulls.
		// For the line indices of the hulls in a 3D scene, see Descriptor<Scene3DAsset>::hullLineIndexCount.
		uint64 dynamicVertexPositionIndexCount;

		uint64 dynamicVertexTextureCoordinateIndexCount;
		uint32 entityCount;
		uint32 hullCount;
		sequence<ref<GeometryAsset>> geometryAssets;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<MaterialComponent>> materialMappings;
	};

	template<>
	struct Descriptor<Scene2DAsset> : public Descriptor<SceneAsset> {
		sequence<StaticTriangle2D> staticTriangles;
		sequence<svec2<float32>> staticVertices;
		sequence<svec2<float32>> staticTextureCoordinates;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct SceneAsset : public Asset {
		public:
			explicit SceneAsset(const Descriptor<SceneAsset>& descriptor) :
				Asset{descriptor.identifier, descriptor.version},
				staticGeometryBuffer(descriptor.staticGeometryBuffer),
				dynamicVertexPositionCount(descriptor.dynamicVertexPositionCount),
				dynamicVertexTextureCoordinateCount(descriptor.dynamicVertexTextureCoordinateCount),
				dynamicVertexPositionIndexCount(descriptor.dynamicVertexPositionIndexCount),
				dynamicVertexTextureCoordinateIndexCount(descriptor.dynamicVertexTextureCoordinateIndexCount),
				entityCount(descriptor.entityCount),
				hullCount(descriptor.hullCount),
				geometryAssets(descriptor.geometryAssets),
				geometryMappings(descriptor.geometryMappings),
				materialMappings(descriptor.materialMappings)
			{}

			inline uint64 getDynamicVertexPositionCount() const {
				return dynamicVertexPositionCount;
			}

			inline uint64 getDynamicVertexTextureCoordinateCount() const {
				return dynamicVertexTextureCoordinateCount;
			}

			inline uint64 getDynamicVertexPositionIndexCount() const {
				return dynamicVertexPositionIndexCount;
			}

			inline uint64 getDynamicVertexTextureCoordinateIndexCount() const {
				return dynamicVertexTextureCoordinateIndexCount;
			}

			inline uint32 getEntityCount() const {
				return entityCount;
			}

			inline uint32 getHullCount() const {
				return hullCount;
			}

			inline const sequence<ref<GeometryAsset>>& getGeometryAssets() const {
				return geometryAssets;
			}

			inline const sequence<EntityComponentMapping<GeometryComponent>>& getGeometryMappings() const {
				return geometryMappings;
			}

			inline const sequence<EntityComponentMapping<MaterialComponent>>& getMaterialMappings() const {
				return materialMappings;
			}

		public:
			sequence<uint8> staticGeometryBuffer;

		private:
			uint64 dynamicVertexPositionCount;
			uint64 dynamicVertexTextureCoordinateCount;
			uint64 dynamicVertexPositionIndexCount;
			uint64 dynamicVertexTextureCoordinateIndexCount;
			uint32 entityCount;
			uint32 hullCount;
			sequence<ref<GeometryAsset>> geometryAssets;
			sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
			sequence<EntityComponentMapping<MaterialComponent>> materialMappings;
	};

	struct Scene2DAsset : public SceneAsset {
		public:
			explicit Scene2DAsset(const Descriptor<Scene2DAsset>& descriptor) :
				SceneAsset(descriptor),
				staticTriangles(descriptor.staticTriangles),
				staticVertices(descriptor.staticVertices),
				staticTextureCoordinates(descriptor.staticTextureCoordinates),
				rigidBodyMappings(descriptor.rigidBodyMappings)
			{}

			inline const sequence<StaticTriangle2D>& getStaticTriangles() const {
				return staticTriangles;
			}

			inline const sequence<svec2<float32>>& getStaticVertices() const {
				return staticVertices;
			}

			inline const sequence<svec2<float32>>& getStaticTextureCoordinates() const {
				return staticTextureCoordinates;
			}

			inline const sequence<EntityComponentMapping<RigidBody2DComponent>>& getRigidBodyMappings() const {
				return rigidBodyMappings;
			}

		private:
			sequence<StaticTriangle2D> staticTriangles;
			sequence<svec2<float32>> staticVertices;
			sequence<svec2<float32>> staticTextureCoordinates;
			sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct Scene3DAsset : public Asset {
		sequence<uint8> geometryData;

		/// The ranges are defined in the following order:
		/// - Vertex
		/// - Position
		/// - Normal
		/// - Texture coordinate
		/// - Lightmap coordinate
		range<uint64, uint64> geometryRanges[5];

		uint64 staticVertexSize;
		uint64 staticPositionSize;
		uint64 staticNormalSize;
		uint64 staticTextureCoordinateSize;

		sequence<Instance3D> instances;
		sequence<range<uint32, uint32>> geometryVertexRanges;
		sequence<MaterialGeometry> materialGeometries;
		sequence<Material> materials;
		sequence<MaterialShader> materialShaders;

		uint32 dynamicMaterialShaderOffset;

		ref<ImageAsset> lightmapAtlasImageAsset;

		float32 skyColor[3];

		uint32 entityCount;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;

		uint32 hullCount = 0;
		uint32 hullEdgeCount = 0;
		uint32 hullFaceCount = 0;
		uint16 hullFaceEdgeIndexCount = 0;
		uint32 hullLineIndexCount = 0;
		uint32 rigidBodyFacePlaneCount = 0;
	};
}