// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	class Scene2DAsset;

	struct Scene2DAssetHeader : public AssetHeader {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 entityCount;
		uint32 geometryAssetCount;
		uint32 geometryMappingCount;
		uint32 rigidBodyMappingCount;
	};

	template<>
	struct Descriptor<Scene2DAsset> : public AssetDescriptor {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
		uint32 entityCount;
		sequence<uint8> staticBuffer;
		sequence<ref<GeometryAsset>> geometryAssets;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};

	class Scene2DAsset : public Asset {
		public:
			explicit Scene2DAsset(const Descriptor<Scene2DAsset>& descriptor) :
				Asset(descriptor),
				staticVertexCount(descriptor.staticVertexCount),
				staticIndexCount(descriptor.staticIndexCount),
				entityCount(descriptor.entityCount),
				staticBuffer(descriptor.staticBuffer),
				geometryAssets(descriptor.geometryAssets),
				geometryMappings(descriptor.geometryMappings),
				rigidBodyMappings(descriptor.rigidBodyMappings)
			{}

			inline uint32 getStaticVertexCount() const {
				return staticVertexCount;
			}

			inline uint32 getStaticIndexCount() const {
				return staticIndexCount;
			}

			inline uint32 getEntityCount() const {
				return entityCount;
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

			inline const sequence<EntityComponentMapping<RigidBody2DComponent>>& getRigidBodyMappings() const {
				return rigidBodyMappings;
			}

		private:
			uint32 staticVertexCount;
			uint32 staticIndexCount;
			uint32 entityCount;
			sequence<uint8> staticBuffer;
			sequence<ref<GeometryAsset>> geometryAssets;
			sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
			sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings;
	};
}