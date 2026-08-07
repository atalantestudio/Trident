// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Scene/Scene2D.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	class Scene2DProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeSceneAsset(uint32 identifier, const BuildScene2D& scene) {
				const uint64 streamSize = calculateStreamSize(scene);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::SCENE);
				stream.write<uint32>(identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.triangles.count()));
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.vertices.count()));
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.textureCoordinates.count()));
				stream.write<uint32>(scene.dynamicGeometry.entityCount);
				stream.write<uint32>(static_cast<uint32>(scene.dynamicGeometry.geometryAssetIdentifiers.count()));
				stream.write<uint32>(static_cast<uint32>(scene.dynamicGeometry.geometryMappings.count()));
				stream.write<uint32>(static_cast<uint32>(scene.dynamicGeometry.rigidBodyMappings.count()));

				for (const StaticTriangle2D& triangle : scene.staticGeometry.triangles) {
					stream.write<uint32>(triangle.aIndex);
					stream.write<uint32>(triangle.bIndex);
					stream.write<uint32>(triangle.cIndex);
					stream.write<uint32>(triangle.aUvIndex);
					stream.write<uint32>(triangle.bUvIndex);
					stream.write<uint32>(triangle.cUvIndex);
				}

				for (const svec2<float32> vertex : scene.staticGeometry.vertices) {
					stream.write<svec2<float32>>(vertex);
				}

				for (const svec2<float32> textureCoordinate : scene.staticGeometry.textureCoordinates) {
					stream.write<svec2<float32>>(textureCoordinate);
				}

				for (const uint32 geometryAssetIdentifier : scene.dynamicGeometry.geometryAssetIdentifiers) {
					stream.write<uint32>(geometryAssetIdentifier);
				}

				for (const EntityComponentMapping<GeometryComponent> geometryMapping : scene.dynamicGeometry.geometryMappings) {
					stream.write<Entity>(geometryMapping.entity);
					stream.write<GeometryComponent>(geometryMapping.component);
				}

				for (const EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : scene.dynamicGeometry.rigidBodyMappings) {
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

			static uint64 calculateStreamSize(const BuildScene2D& scene) {
				return (
					sizeof(AssetHeader) +
					sizeof(uint32) * 7 +
					scene.staticGeometry.triangles.count() * sizeof(StaticTriangle2D) +
					scene.staticGeometry.vertices.count() * sizeof(svec2<float32>) +
					scene.staticGeometry.textureCoordinates.count() * sizeof(svec2<float32>) +
					scene.dynamicGeometry.geometryAssetIdentifiers.count() * sizeof(uint32) +
					scene.dynamicGeometry.geometryMappings.count() * sizeof(EntityComponentMapping<GeometryComponent>) +
					scene.dynamicGeometry.rigidBodyMappings.count() * (sizeof(Entity) + sizeof(svec2<float32>) * 3 + sizeof(float32) * 6)
				);
			}

		public:
			sint32 main(view<void*> arguments) final override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path finalAssetOutputPath = assetOutputPath / std::string(assetName);
				finalAssetOutputPath.replace_extension("tsa");

				if (std::filesystem::exists(finalAssetOutputPath)) {
					ENGINE_INFO("Scene asset \"[]\" already exists.", finalAssetOutputPath.filename().string());

					return 0;
				}

				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const BuildScene2D scene = createScene(assetLibrary);
				const ByteStream<StreamOperation::WRITE> stream = encodeSceneAsset(assetIdentifier, scene);

				createSceneAsset(assetIdentifier, stream, assetName, assetOutputPath);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		protected:
			explicit Scene2DProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual BuildScene2D createScene(AssetBuilderLibrary* assetLibrary) const = 0;

		private:
			void createSceneAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				assetLibrary->registerAsset(identifier, assetName);
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}