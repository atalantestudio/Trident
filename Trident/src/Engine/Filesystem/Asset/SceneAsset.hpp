// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedRigidBody2D.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct Scene2DAssetHeader : public AssetHeader {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 dynamicVertexCount;
		uint32 dynamicIndexCount;
		uint32 entityCount;
		uint32 geometryCount;
		uint32 rigidBodyCount;
		uint32 geometryComponentCount;
	};

	struct Scene2DAssetDescriptor : public AssetDescriptor {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 dynamicVertexCount;
		uint32 dynamicIndexCount;
		uint32 entityCount;
		sequence<uint8> vertexAndIndexBuffer;
		sequence<CachedGeometry2D> geometries;
		sequence<CachedRigidBody2D> rigidBodies;
		sequence<GeometryComponent> geometryComponents;
	};

	class Scene2DAsset : public Asset {
		public:
			explicit Scene2DAsset(const Scene2DAssetDescriptor& descriptor) :
				Asset(descriptor),
				staticVertexCount(descriptor.staticVertexCount),
				staticIndexCount(descriptor.staticIndexCount),
				dynamicVertexCount(descriptor.dynamicVertexCount),
				dynamicIndexCount(descriptor.dynamicIndexCount),
				entityCount(descriptor.entityCount),
				vertexAndIndexBuffer(descriptor.vertexAndIndexBuffer),
				geometries(descriptor.geometries.count()),
				rigidBodies(descriptor.rigidBodies),
				geometryComponents(descriptor.geometryComponents)
			{
				const view<float32> vertexBuffer(pointer_cast<const float32*>(&vertexAndIndexBuffer[0]), (staticVertexCount + dynamicVertexCount) * 2);
				const view<uint16> indexBuffer(pointer_cast<const uint16*>(&vertexAndIndexBuffer[(staticVertexCount + dynamicVertexCount) * sizeof(svec2<float32>)]), staticIndexCount + dynamicIndexCount);

				uint32 geometryIndex = 0;

				while (geometryIndex < geometries.count()) {
					geometries[geometryIndex] = Geometry2D::fromCached(descriptor.geometries[geometryIndex], vertexBuffer, indexBuffer);
					geometryIndex += 1;
				}
			}

			uint32 getStaticVertexCount() const {
				return staticVertexCount;
			}

			uint32 getStaticIndexCount() const {
				return staticIndexCount;
			}

			uint32 getDynamicVertexCount() const {
				return dynamicVertexCount;
			}

			uint32 getDynamicIndexCount() const {
				return dynamicIndexCount;
			}

			uint32 getEntityCount() const {
				return entityCount;
			}

			const sequence<uint8>& getVertexAndIndexBuffer() const {
				return vertexAndIndexBuffer;
			}

			const sequence<Geometry2D>& getGeometries() const {
				return geometries;
			}

			const sequence<CachedRigidBody2D>& getRigidBodies() const {
				return rigidBodies;
			}

			const sequence<GeometryComponent>& getGeometryComponents() const {
				return geometryComponents;
			}

		private:
			uint32 staticVertexCount;
			uint32 staticIndexCount;
			uint32 dynamicVertexCount;
			uint32 dynamicIndexCount;
			uint32 entityCount;
			sequence<uint8> vertexAndIndexBuffer;
			sequence<Geometry2D> geometries;
			sequence<CachedRigidBody2D> rigidBodies;
			sequence<GeometryComponent> geometryComponents;
	};
}