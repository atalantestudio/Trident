// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Cached/CachedGeometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedRigidBody2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct Scene2DAssetHeader : public AssetHeader {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 dynamicVertexCount;
		uint32 dynamicIndexCount;
		uint32 geometryCount;
		uint32 rigidBodyCount;
	};

	struct Scene2DAssetDescriptor : public AssetDescriptor {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 dynamicVertexCount;
		uint32 dynamicIndexCount;
		sequence<float32> vertexBuffer;
		sequence<uint16> indexBuffer;
		sequence<CachedGeometry2D> geometries;
		sequence<CachedRigidBody2D> rigidBodies;
	};

	class Scene2DAsset : public Asset {
		public:
			explicit Scene2DAsset(const Scene2DAssetDescriptor& descriptor) :
				Asset(descriptor),
				staticVertexCount(descriptor.staticVertexCount),
				staticIndexCount(descriptor.staticIndexCount),
				dynamicVertexCount(descriptor.dynamicVertexCount),
				dynamicIndexCount(descriptor.dynamicIndexCount),
				vertexBuffer(descriptor.vertexBuffer),
				indexBuffer(descriptor.indexBuffer),
				geometries(descriptor.geometries),
				rigidBodies(descriptor.rigidBodies)
			{}

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

			const sequence<float32>& getVertexBuffer() const {
				return vertexBuffer;
			}

			const sequence<uint16>& getIndexBuffer() const {
				return indexBuffer;
			}

			const sequence<CachedGeometry2D>& getGeometries() const {
				return geometries;
			}

			const sequence<CachedRigidBody2D>& getRigidBodies() const {
				return rigidBodies;
			}

		private:
			uint32 staticVertexCount;
			uint32 staticIndexCount;
			uint32 dynamicVertexCount;
			uint32 dynamicIndexCount;
			sequence<float32> vertexBuffer;
			sequence<uint16> indexBuffer;
			sequence<CachedGeometry2D> geometries;
			sequence<CachedRigidBody2D> rigidBodies;
	};
}