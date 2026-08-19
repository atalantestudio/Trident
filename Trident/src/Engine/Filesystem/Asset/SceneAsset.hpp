// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Instance.hpp"
#include "Engine/API/Light/PointLight.hpp"
#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialShader.hpp"
#include "Engine/API/Material/StaticMaterial.hpp"
#include "Engine/API/Scene/StaticTriangle2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct Scene2DAsset;
	struct Scene3DAsset;

	template<>
	struct Descriptor<Scene2DAsset>: public Asset {
		sequence<ref<Geometry2DAsset>> geometryAssets;
		sequence<StaticTriangle2D> staticTriangles;
		sequence<svec2<float32>> staticVertices;
		sequence<svec2<float32>> staticTextureCoordinates;
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
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct Scene2DAsset : public Asset {
		public:
			Scene2DAsset() = default;

			explicit Scene2DAsset(const Descriptor<Scene2DAsset>& descriptor) :
				Asset(descriptor),
				geometryAssets(descriptor.geometryAssets),
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

			inline const sequence<EntityComponentMapping<GeometryComponent>>& getGeometryMappings() const {
				return geometryMappings;
			}

			inline const sequence<EntityComponentMapping<RigidBody2DComponent>>& getRigidBodyMappings() const {
				return rigidBodyMappings;
			}

		public:
			sequence<uint8> staticGeometryBuffer;
			sequence<ref<Geometry2DAsset>> geometryAssets;

		private:
			sequence<StaticTriangle2D> staticTriangles;
			sequence<svec2<float32>> staticVertices;
			sequence<svec2<float32>> staticTextureCoordinates;
			uint64 dynamicVertexPositionCount;
			uint64 dynamicVertexTextureCoordinateCount;
			uint64 dynamicVertexPositionIndexCount;
			uint64 dynamicVertexTextureCoordinateIndexCount;
			uint32 entityCount;
			uint32 hullCount;
			sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
			sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	struct Scene3DAsset : public Asset {
		sequence<uint8> geometryData;

		/// Vertices, indices, lightmap coordinates.
		range<uint64, uint64> geometryRanges[3];

		uint64 staticVertexSize;
		uint64 staticPositionSize;
		uint64 staticNormalSize;
		uint64 staticTextureCoordinateSize;

		sequence<Instance3D> instances;
		sequence<ref<Geometry3DAsset>> geometryAssets;
		sequence<range<uint32, uint32>> geometryIndexRanges;
		sequence<MaterialGeometry> materialGeometries;
		sequence<Material> materials;
		sequence<MaterialShader> materialShaders;
		sequence<PointLight> pointLights;

		uint32 dynamicMaterialShaderOffset;

		ref<ImageAsset> lightmapAtlasImageAsset;

		svec3<float32> skyColor;

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