// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/API/Process/Process.hpp"

#include "../Subprocess/SceneData.hpp"
#include "../Subprocess/Scene2D.hpp"
#include "../Subprocess/Scene3D.hpp"

namespace Trident {
	class BuildSceneAssetProcess final : public Process {
		private:
			typedef Scene2DData (*CreateScene2DFunction)();
			typedef Scene3DData (*CreateScene3DFunction)();

		private:
			static constexpr uint32 SCENE_2D_COUNT = 9;
			static constexpr uint32 SCENE_3D_COUNT = 2;

			inline static const view<char8> SCENE_2D_NAMES[SCENE_2D_COUNT] = {
				"Boxes",
				"Circles",
				"Dominos",
				"Polygons",
				"Pyramid",
				"Stacking",
				"RollingBall",
				"SlidingBoxes",
				"Swing",
			};

			inline static constexpr CreateScene2DFunction SCENE_2D_FUNCTIONS[SCENE_2D_COUNT] = {
				createBoxesScene,
				createCirclesScene,
				createDominosScene,
				createPolygonsScene,
				createPyramidScene,
				createStackingScene,
				createRollingBallScene,
				createSlidingBoxesScene,
				createSwingScene,
			};

			inline static const view<char8> SCENE_3D_NAMES[SCENE_3D_COUNT] = {
				"Box",
				"Lemons",
			};

			inline static constexpr CreateScene3DFunction SCENE_3D_FUNCTIONS[SCENE_3D_COUNT] = {
				createBoxScene,
				createLemonsScene,
			};

		private:
			static ByteStream<StreamOperation::WRITE> encodeScene2DAsset(uint32 identifier, const Scene2DData& data) {
				SceneAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticVertexCount = data.staticVertexCount;
				header.staticIndexCount = data.staticIndexCount;
				header.entityCount = data.entityCount;
				header.geometryAssetCount = static_cast<uint32>(data.geometryAssetIdentifiers.count());
				header.geometryMappingCount = static_cast<uint32>(data.geometryMappings.count());
				header.materialMappingCount = 0;
				header.rigidBodyMappingCount = static_cast<uint32>(data.rigidBodyMappings.count());

				const uint64 size =
					sizeof(header) +
					header.staticVertexCount * sizeof(svec2<float32>) +
					header.staticIndexCount * sizeof(uint32) +
					header.geometryAssetCount * sizeof(uint32) +
					header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.materialMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec2<float32>) * 3 + sizeof(float32) * 6);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());

				stream.jump(sizeof(header));

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

			static ByteStream<StreamOperation::WRITE> encodeScene3DAsset(uint32 identifier, const Scene3DData& data) {
				SceneAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticVertexCount = data.staticVertexCount;
				header.staticIndexCount = data.staticIndexCount;
				header.entityCount = data.entityCount;
				header.geometryAssetCount = static_cast<uint32>(data.geometryAssetIdentifiers.count());
				header.geometryMappingCount = static_cast<uint32>(data.geometryMappings.count());
				header.materialMappingCount = static_cast<uint32>(data.materialMappings.count());
				header.rigidBodyMappingCount = static_cast<uint32>(data.rigidBodyMappings.count());

				const uint64 size =
					sizeof(header) +
					header.staticVertexCount * sizeof(svec2<float32>) +
					header.staticIndexCount * sizeof(uint32) +
					header.geometryAssetCount * sizeof(uint32) +
					header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.materialMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec2<float32>) * 2 + sizeof(float32) * 5);

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());

				stream.jump(sizeof(header));

				for (const uint32 geometryAssetIdentifier : data.geometryAssetIdentifiers) {
					stream.write<uint32>(geometryAssetIdentifier);
				}

				for (const EntityComponentMapping<GeometryComponent> geometryMapping : data.geometryMappings) {
					stream.write<Entity>(geometryMapping.entity);
					stream.write<uint32>(geometryMapping.component.geometryIndex);
				}

				for (const EntityComponentMapping<MaterialComponent> materialMapping : data.materialMappings) {
					stream.write<Entity>(materialMapping.entity);
					stream.write<uint32>(materialMapping.component.materialIndex);
				}

				for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : data.rigidBodyMappings) {
					stream.write<Entity>(rigidBodyMapping.entity);
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.position));
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.rotation));
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.scale));
				}

				return stream;
			}

			static void encodeSceneAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", outputPath, identifier);
			}

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";

				uint32 sceneIndex = 0;

				while (sceneIndex < SCENE_2D_COUNT) {
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const Scene2DData sceneData = SCENE_2D_FUNCTIONS[sceneIndex]();
					const ByteStream<StreamOperation::WRITE> stream = encodeScene2DAsset(identifier, sceneData);

					encodeSceneAsset(identifier, stream, SCENE_2D_NAMES[sceneIndex], assetOutputPath);

					sceneIndex += 1;
				}

				sceneIndex = 0;

				while (sceneIndex < SCENE_3D_COUNT) {
					const uint32 identifier = AssetLibrary::createAssetIdentifier();
					const Scene3DData sceneData = SCENE_3D_FUNCTIONS[sceneIndex]();
					const ByteStream<StreamOperation::WRITE> stream = encodeScene3DAsset(identifier, sceneData);

					encodeSceneAsset(identifier, stream, SCENE_3D_NAMES[sceneIndex], assetOutputPath);

					sceneIndex += 1;
				}

				ENGINE_INFO("Done building scene assets.");

				return 0;
			}
	};
}