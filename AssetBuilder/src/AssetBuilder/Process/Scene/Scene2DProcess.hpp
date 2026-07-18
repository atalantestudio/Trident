// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/BuildAssetMap.hpp"
#include "../../../Subprocess/SceneData.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class Scene2DProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeScene2DAsset(uint32 identifier, const Scene2DData& data) {
				SceneAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticTriangleCount = static_cast<uint32>(data.staticScene.triangles.count());
				header.staticVertexCount = static_cast<uint32>(data.staticScene.vertices.count());
				header.staticTextureCoordinateCount = static_cast<uint32>(data.staticScene.textureCoordinates.count());
				header.staticLightmapCoordinateCount = 0;
				header.entityCount = data.entityCount;
				header.geometryAssetCount = static_cast<uint32>(data.geometryAssetIdentifiers.count());
				header.geometryMappingCount = static_cast<uint32>(data.geometryMappings.count());
				header.materialMappingCount = 0;
				header.rigidBodyMappingCount = static_cast<uint32>(data.rigidBodyMappings.count());

				const uint64 size =
					sizeof(header) +
					header.staticTriangleCount * sizeof(StaticTriangle2D) +
					header.staticVertexCount * sizeof(svec2<float32>) +
					header.staticTextureCoordinateCount * sizeof(svec2<float32>) +
					header.geometryAssetCount * sizeof(uint32) +
					header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.materialMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec2<float32>) * 3 + sizeof(float32) * 6);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());

				stream.jump(sizeof(header));

				for (const StaticTriangle2D& triangle : data.staticScene.triangles) {
					stream.write<uint32>(triangle.aIndex);
					stream.write<uint32>(triangle.bIndex);
					stream.write<uint32>(triangle.cIndex);
					stream.write<uint32>(triangle.aUvIndex);
					stream.write<uint32>(triangle.bUvIndex);
					stream.write<uint32>(triangle.cUvIndex);
				}

				for (const svec2<float32> vertex : data.staticScene.vertices) {
					stream.write<svec2<float32>>(vertex);
				}

				for (const svec2<float32> textureCoordinate : data.staticScene.textureCoordinates) {
					stream.write<svec2<float32>>(textureCoordinate);
				}

				for (const uint32 geometryAssetIdentifier : data.geometryAssetIdentifiers) {
					stream.write<uint32>(geometryAssetIdentifier);
				}

				for (const EntityComponentMapping<GeometryComponent> geometryMapping : data.geometryMappings) {
					stream.write<Entity>(geometryMapping.entity);
					stream.write<uint32>(geometryMapping.component.geometryIndex);
				}

				for (const EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : data.rigidBodyMappings) {
					stream.write<Entity>(rigidBodyMapping.entity);
					stream.write<svec2<float32>>(svec2<float32>(rigidBodyMapping.component.position));
					stream.write<float32>(rigidBodyMapping.component.rotation);
					stream.write<svec2<float32>>(svec2<float32>(rigidBodyMapping.component.scale));
					stream.write<svec2<float32>>(svec2<float32>(rigidBodyMapping.component.linearVelocity));
					stream.write<float32>(rigidBodyMapping.component.angularVelocity);
					stream.write<float32>(rigidBodyMapping.component.inverseMass);
					stream.write<float32>(rigidBodyMapping.component.inverseInertia);
					stream.write<float32>(rigidBodyMapping.component.friction);
					stream.write<float32>(rigidBodyMapping.component.restitution);
				}

				return stream;
			}

			void createSceneAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				assetMap->insert(identifier, assetName);
			}

		protected:
			explicit Scene2DProcess(BuildAssetMap* assetMap) :
				assetMap(assetMap)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual Scene2DData createScene(std::filesystem::path rootOutputPath) const = 0;

		public:
			sint32 main(view<void*> arguments) final override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";
				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const view<char8> assetName = getAssetName();
				const Scene2DData scene = createScene(rootOutputPath);
				const ByteStream<StreamOperation::WRITE> stream = encodeScene2DAsset(assetIdentifier, scene);

				createSceneAsset(assetIdentifier, stream, assetName, assetOutputPath);

				ENGINE_INFO("Generated 2D scene asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		private:
			BuildAssetMap* assetMap;
	};
}