// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Scene/StaticVertex.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	class SceneAsset;
	class Scene2DAsset;
	class Scene3DAsset;

	struct SceneAssetHeader : public AssetHeader {
		uint32 staticTriangleCount;
		uint32 staticVertexCount;
		uint32 staticNormalCount;
		uint32 staticTextureCoordinateCount;
		uint32 entityCount;
		uint32 geometryAssetCount;
		uint32 geometryMappingCount;
		uint32 materialMappingCount;
		uint32 rigidBodyMappingCount;
	};

	template<>
	struct Descriptor<SceneAsset> : public AssetDescriptor {
		uint64 dynamicVertexPositionCount;
		uint64 dynamicVertexTextureCoordinateCount;

		// NOTE
		// In 2D, contains the line indices of the hulls.
		// For the line indices of the hulls in a 3D scene, see Descriptor<Scene3DAsset>::hullLineIndexCount.
		uint64 dynamicVertexPositionIndexCount;

		uint64 dynamicVertexTextureCoordinateIndexCount;
		uint32 entityCount;
		uint32 hullCount;
		sequence<uint8> staticBuffer;
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

	template<>
	struct Descriptor<Scene3DAsset> : public Descriptor<SceneAsset> {
		uint64 dynamicVertexNormalCount;
		uint64 dynamicTriangleTangentCount;
		uint64 dynamicVertexNormalIndexCount;
		uint32 hullEdgeCount;
		uint32 hullFaceCount;
		uint16 hullFaceEdgeIndexCount;
		uint32 hullLineIndexCount;
		uint32 rigidBodyFacePlaneCount;
		sequence<StaticTriangle3D> staticTriangles;
		sequence<svec3<float32>> staticVertices;
		sequence<svec3<float32>> staticNormals;
		sequence<svec2<float32>> staticTextureCoordinates;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};

	class SceneAsset : public Asset {
		public:
			explicit SceneAsset(const Descriptor<SceneAsset>& descriptor) :
				Asset(descriptor),
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

			inline const sequence<uint8>& getStaticBuffer() const {
				return staticBuffer;
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

		private:
			uint64 dynamicVertexPositionCount;
			uint64 dynamicVertexTextureCoordinateCount;
			uint64 dynamicVertexPositionIndexCount;
			uint64 dynamicVertexTextureCoordinateIndexCount;
			uint32 entityCount;
			uint32 hullCount;
			sequence<uint8> staticBuffer;
			sequence<ref<GeometryAsset>> geometryAssets;
			sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
			sequence<EntityComponentMapping<MaterialComponent>> materialMappings;
	};

	class Scene2DAsset : public SceneAsset {
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

	class Scene3DAsset : public SceneAsset {
		public:
			explicit Scene3DAsset(const Descriptor<Scene3DAsset>& descriptor) :
				SceneAsset(descriptor),
				dynamicVertexNormalCount(descriptor.dynamicVertexNormalCount),
				dynamicTriangleTangentCount(descriptor.dynamicTriangleTangentCount),
				dynamicVertexNormalIndexCount(descriptor.dynamicVertexNormalIndexCount),
				hullEdgeCount(descriptor.hullEdgeCount),
				hullFaceCount(descriptor.hullFaceCount),
				hullFaceEdgeIndexCount(descriptor.hullFaceEdgeIndexCount),
				hullLineIndexCount(descriptor.hullLineIndexCount),
				staticTriangles(descriptor.staticTriangles),
				staticVertices(descriptor.staticVertices),
				staticNormals(descriptor.staticNormals),
				staticTextureCoordinates(descriptor.staticTextureCoordinates),
				rigidBodyFacePlaneCount(descriptor.rigidBodyFacePlaneCount),
				rigidBodyMappings(descriptor.rigidBodyMappings)
			{}

			inline const sequence<StaticTriangle3D>& getStaticTriangles() const {
				return staticTriangles;
			}

			inline const sequence<svec3<float32>>& getStaticVertices() const {
				return staticVertices;
			}

			inline const sequence<svec3<float32>>& getStaticNormals() const {
				return staticNormals;
			}

			inline const sequence<svec2<float32>>& getStaticTextureCoordinates() const {
				return staticTextureCoordinates;
			}

			inline uint64 getDynamicVertexNormalCount() const {
				return dynamicVertexNormalCount;
			}

			inline uint64 getDynamicTriangleTangentCount() const {
				return dynamicTriangleTangentCount;
			}

			inline uint64 getDynamicVertexNormalIndexCount() const {
				return dynamicVertexNormalIndexCount;
			}

			inline uint32 getHullEdgeCount() const {
				return hullEdgeCount;
			}

			inline uint32 getHullFaceCount() const {
				return hullFaceCount;
			}

			inline uint16 getHullFaceEdgeIndexCount() const {
				return hullFaceEdgeIndexCount;
			}

			inline uint32 getHullLineIndexCount() const {
				return hullLineIndexCount;
			}

			inline uint32 getRigidBodyFacePlaneCount() const {
				return rigidBodyFacePlaneCount;
			}

			inline const sequence<EntityComponentMapping<RigidBody3DComponent>>& getRigidBodyMappings() const {
				return rigidBodyMappings;
			}

		private:
			uint64 dynamicVertexNormalCount;
			uint64 dynamicTriangleTangentCount;
			uint64 dynamicVertexNormalIndexCount;
			uint32 hullEdgeCount;
			uint32 hullFaceCount;
			uint16 hullFaceEdgeIndexCount;
			uint32 hullLineIndexCount;
			sequence<StaticTriangle3D> staticTriangles;
			sequence<svec3<float32>> staticVertices;
			sequence<svec3<float32>> staticNormals;
			sequence<svec2<float32>> staticTextureCoordinates;
			uint32 rigidBodyFacePlaneCount;
			sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};
}