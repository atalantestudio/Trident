// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "../Subprocess/SceneData.hpp"
#include "../Subprocess/Scene2D.hpp"
#include "../Subprocess/Scene3D.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	// TODO(asset-builder/scene): Deprecate. Use individual processes extending SceneProcess instead.
	class BuildSceneAssetProcess final : public Process {
		private:
			typedef Scene2DData (*CreateScene2DFunction)();
			typedef Scene3DData (*CreateScene3DFunction)(std::filesystem::path rootOutputPath);

		private:
			static constexpr uint32 SCENE_2D_COUNT = 9;
			static constexpr uint32 SCENE_3D_COUNT = 5;

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
				"Boxes3D",
				"BoxStacking",
				"EdgeContact",
				"Lemons",
				"PlayerMovement",
			};

			inline static constexpr CreateScene3DFunction SCENE_3D_FUNCTIONS[SCENE_3D_COUNT] = {
				createBoxes3DScene,
				createBoxStackingScene,
				createEdgeContactScene,
				createLemonsScene,
				createPlayerMovementScene,
			};

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

			static ByteStream<StreamOperation::WRITE> encodeScene3DAsset(uint32 identifier, const Scene3DData& data) {
				SceneAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticTriangleCount = static_cast<uint32>(data.staticScene.triangles.count());
				header.staticVertexCount = static_cast<uint32>(data.staticScene.vertices.count());
				header.staticNormalCount = static_cast<uint32>(data.staticScene.normals.count());
				header.staticTextureCoordinateCount = static_cast<uint32>(data.staticScene.textureCoordinates.count());
				header.staticLightmapCoordinateCount = static_cast<uint32>(data.staticScene.lightmapCoordinates.count());
				header.entityCount = data.entityCount;
				header.geometryAssetCount = static_cast<uint32>(data.geometryAssetIdentifiers.count());
				header.geometryMappingCount = static_cast<uint32>(data.geometryMappings.count());
				header.materialMappingCount = static_cast<uint32>(data.materialMappings.count());
				header.rigidBodyMappingCount = static_cast<uint32>(data.rigidBodyMappings.count());

				uint64 size = (
					sizeof(header) + 3 * sizeof(uint32) +
					header.staticTriangleCount * sizeof(StaticTriangle3D) +
					header.staticVertexCount * sizeof(svec3<float32>) +
					header.staticNormalCount * sizeof(svec3<float32>) +
					header.staticTextureCoordinateCount * sizeof(svec2<float32>) +
					header.staticLightmapCoordinateCount * sizeof(svec2<float32>) +
					data.staticScene.materials.count() * (sizeof(uint32) * 4) +
					header.geometryAssetCount * sizeof(uint32) +
					header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.materialMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec3<float32>) * 7 + sizeof(float32) * 4)
				);

				for (const BuildMaterialShader& materialShader : data.staticScene.materialShaders) {
					size += (
						6 * sizeof(uint32) +
						sizeof(VersionNumber) +
						materialShader.name.count() +
						sizeof(ShaderStage) +
						materialShader.entryPoint.count() +
						materialShader.descriptorSetLayoutBindings.count() * (sizeof(DescriptorType) + sizeof(uint32) + sizeof(ShaderStage))
					);
				}

				ByteStream<StreamOperation::WRITE> stream(size, Endianness::LITTLE_ENDIAN);

				std::copy_n(pointer_cast<const uint8*>(&header), sizeof(header), stream.getBytePosition());

				stream.jump(sizeof(header));
				stream.write<uint32>(data.staticScene.lightmapAtlasImageAssetDescriptor.identifier);
				stream.write<uint32>(static_cast<uint32>(data.staticScene.materialShaders.count()));
				stream.write<uint32>(static_cast<uint32>(data.staticScene.materials.count()));

				for (const StaticTriangle3D& triangle : data.staticScene.triangles) {
					stream.write<uint32>(triangle.vertices[0].vertexIndex);
					stream.write<uint32>(triangle.vertices[0].textureCoordinateIndex);
					stream.write<uint32>(triangle.vertices[0].lightmapCoordinateIndex);
					stream.write<uint32>(triangle.vertices[1].vertexIndex);
					stream.write<uint32>(triangle.vertices[1].textureCoordinateIndex);
					stream.write<uint32>(triangle.vertices[1].lightmapCoordinateIndex);
					stream.write<uint32>(triangle.vertices[2].vertexIndex);
					stream.write<uint32>(triangle.vertices[2].textureCoordinateIndex);
					stream.write<uint32>(triangle.vertices[2].lightmapCoordinateIndex);
					stream.write<uint32>(triangle.normalIndex);
				}

				for (const svec3<float32> vertex : data.staticScene.vertices) {
					stream.write<svec3<float32>>(vertex);
				}

				for (const svec3<float32> normal : data.staticScene.normals) {
					stream.write<svec3<float32>>(normal);
				}

				for (const svec2<float32> textureCoordinate : data.staticScene.textureCoordinates) {
					stream.write<svec2<float32>>(textureCoordinate);
				}

				for (const svec2<float32> lightmapCoordinate : data.staticScene.lightmapCoordinates) {
					stream.write<svec2<float32>>(lightmapCoordinate);
				}

				for (const BuildMaterialShader& materialShader : data.staticScene.materialShaders) {
					stream.write<uint32>(0);
					stream.write<VersionNumber>({0, 0, 0});
					stream.write<uint32>(static_cast<uint32>(materialShader.name.count()));

					std::copy_n(&materialShader.name[0], materialShader.name.count(), pointer_cast<char8*>(stream.getBytePosition()));

					stream.jump(materialShader.name.count());
					stream.write<ShaderStage>(materialShader.stage);
					stream.write<uint32>(static_cast<uint32>(materialShader.entryPoint.count()));

					std::copy_n(&materialShader.entryPoint[0], materialShader.entryPoint.count(), pointer_cast<char8*>(stream.getBytePosition()));

					stream.jump(materialShader.entryPoint.count());
					stream.write<uint32>(static_cast<uint32>(materialShader.descriptorSetLayoutBindings.count()));

					for (const DescriptorSetLayoutBinding& binding : materialShader.descriptorSetLayoutBindings) {
						stream.write<DescriptorType>(binding.descriptorType);
						stream.write<uint32>(binding.descriptorCount);
						stream.write<ShaderStage>(binding.shaderStages);
					}

					stream.write<uint32>(materialShader.materialAssetRange.offset);
					stream.write<uint32>(materialShader.materialAssetRange.count);
				}

				for (const BuildStaticMaterial3D& staticMaterial : data.staticScene.materials) {
					stream.write<uint32>(staticMaterial.identifier);
				}

				for (const BuildStaticMaterial3D& staticMaterial : data.staticScene.materials) {
					stream.write<uint32>(staticMaterial.shaderIndex);
					stream.write<uint32>(staticMaterial.triangleRange.offset);
					stream.write<uint32>(staticMaterial.triangleRange.count);
				}

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
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.linearVelocity));
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.angularVelocity));
					stream.write<float32>(rigidBodyMapping.component.scale);
					stream.write<float32>(rigidBodyMapping.component.inverseMass);
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.inverseInertia[0]));
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.inverseInertia[1]));
					stream.write<svec3<float32>>(svec3<float32>(rigidBodyMapping.component.inverseInertia[2]));
					stream.write<float32>(rigidBodyMapping.component.friction);
					stream.write<float32>(rigidBodyMapping.component.restitution);
				}

				return stream;
			}

			void encodeSceneAsset(uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", outputPath, identifier);

				assetLibrary->registerAsset(identifier, assetName);
			}

			void encodeScene3DAsset(const Scene3DData& sceneData, uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& assetOutputPath) {
				encodeSceneAsset(identifier, stream, assetName, assetOutputPath);

				if (sceneData.staticScene.lightmapAtlasImageAssetDescriptor.identifier != max<uint32>()) {
					assetLibrary->registerAsset(sceneData.staticScene.lightmapAtlasImageAssetDescriptor.identifier, assetName);
				}
			}

		public:
			explicit BuildSceneAssetProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
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
					const Scene3DData sceneData = SCENE_3D_FUNCTIONS[sceneIndex](rootOutputPath);
					const ByteStream<StreamOperation::WRITE> stream = encodeScene3DAsset(identifier, sceneData);

					encodeScene3DAsset(sceneData, identifier, stream, SCENE_3D_NAMES[sceneIndex], assetOutputPath);

					sceneIndex += 1;
				}

				ENGINE_INFO("Done building scene assets.");

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}