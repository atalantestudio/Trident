// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Scene/StaticScene.hpp"
#include "../../../Process/BuildImageAssetProcess.hpp"

#include "Engine/API/Process/Process.hpp"
#include "Engine/ECS/Component/GeometryComponent.hpp"
#include "Engine/ECS/Component/MaterialComponent.hpp"
#include "Engine/ECS/Component/RigidBody3DComponent.hpp"
#include "Engine/ECS/EntityComponentMapping.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	struct Scene3DData {
		StaticScene3D staticScene;
		uint32 entityCount;
		sequence<uint32> geometryAssetIdentifiers;
		sequence<BuildMaterialShader> materialShaders;
		sequence<ref<MaterialAsset>> materialAssets;
		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings;
		sequence<EntityComponentMapping<MaterialComponent>> materialMappings;
		sequence<EntityComponentMapping<RigidBody3DComponent>> rigidBodyMappings;
	};

	class Scene3DProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeScene3DAsset(uint32 identifier, const Scene3DData& data) {
				SceneAssetHeader header{};
				header.signature = AssetSignature::SCENE;
				header.identifier = identifier;
				header.version = {0, 0, 0};
				header.staticTriangleCount = static_cast<uint32>(data.staticScene.indices.count()) / 3;
				header.staticVertexCount = static_cast<uint32>(data.staticScene.vertices.count());
				header.staticTextureCoordinateCount = static_cast<uint32>(data.staticScene.textureCoordinates.count());
				header.entityCount = data.entityCount;
				header.geometryAssetCount = static_cast<uint32>(data.geometryAssetIdentifiers.count());
				header.geometryMappingCount = static_cast<uint32>(data.geometryMappings.count());
				header.materialMappingCount = static_cast<uint32>(data.materialMappings.count());
				header.rigidBodyMappingCount = static_cast<uint32>(data.rigidBodyMappings.count());

				const uint32 staticNormalCount = static_cast<uint32>(data.staticScene.normals.count());

				uint64 size = (
					sizeof(header) + 5 * sizeof(uint32) +
					header.staticTriangleCount * (sizeof(StaticVertex3D) - sizeof(uint32)) * 3 +
					header.staticVertexCount * sizeof(svec3<float32>) +
					staticNormalCount * sizeof(svec3<float32>) +
					header.staticTextureCoordinateCount * sizeof(svec2<float32>) +
					(data.staticScene.materials.count() + data.materialAssets.count()) * sizeof(uint32) +
					data.staticScene.materials.count() * sizeof(uint32) * 3 +
					header.geometryAssetCount * sizeof(uint32) +
					header.geometryMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.materialMappingCount * (sizeof(Entity) + sizeof(uint32)) +
					header.rigidBodyMappingCount * (sizeof(Entity) + sizeof(svec3<float32>) * 7 + sizeof(float32) * 4)
				);

				sequence<BuildMaterialShader> materialShaders(data.staticScene.materialShaders.count() + data.materialShaders.count());

				std::copy(data.staticScene.materialShaders.begin(), data.staticScene.materialShaders.end(), &materialShaders[0]);

				// TODO(asset-builder/scene): Ensure all scenes use materials.
				if (data.materialShaders.count() > 0) {
					ENGINE_WARNING("The scene doesn't use any material shaders.");

					std::copy(data.materialShaders.begin(), data.materialShaders.end(), &materialShaders[data.staticScene.materialShaders.count()]);
				}

				for (const BuildMaterialShader& materialShader : materialShaders) {
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
				stream.write<uint32>(staticNormalCount);
				stream.write<uint32>(data.staticScene.lightmapAtlasImageAssetDescriptor.identifier);
				stream.write<uint32>(static_cast<uint32>(materialShaders.count()));
				stream.write<uint32>(static_cast<uint32>(data.materialAssets.count() + data.staticScene.materials.count()));
				stream.write<uint32>(static_cast<uint32>(data.staticScene.materials.count()));

				for (const StaticVertex3D& index : data.staticScene.indices) {
					stream.write<svec2<float32>>(index.lightmapCoordinate);
					stream.write<uint32>(index.vertexIndex);
					stream.write<uint32>(index.normalIndex);
					stream.write<uint32>(index.textureCoordinateIndex);
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

				for (const BuildMaterialShader& materialShader : materialShaders) {
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

				for (const ref<MaterialAsset> materialAsset : data.materialAssets) {
					stream.write<uint32>(materialAsset->identifier);
				}

				for (const BuildStaticMaterial3D& staticMaterial : data.staticScene.materials) {
					stream.write<uint32>(staticMaterial.shaderIndex);
					stream.write<uint32>(staticMaterial.indexRange.offset);
					stream.write<uint32>(staticMaterial.indexRange.count);
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

			void createSceneAsset(Scene3DData& sceneData, uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& rootOutputPath) {
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.size);

				assetLibrary->registerAsset(identifier, assetName);

				BuildImageAssetProcess::encodeImageAsset(rootOutputPath / "Asset/", assetName, sceneData.staticScene.lightmapAtlasImageAssetDescriptor);

				assetLibrary->registerAsset(sceneData.staticScene.lightmapAtlasImageAssetDescriptor.identifier, assetName);
			}

		protected:
			explicit Scene3DProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const = 0;

		public:
			sint32 main(view<void*> arguments) final override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const view<char8> assetName = getAssetName();

				Scene3DData scene = createScene(assetLibrary);

				const ByteStream<StreamOperation::WRITE> stream = encodeScene3DAsset(assetIdentifier, scene);

				createSceneAsset(scene, assetIdentifier, stream, assetName, rootOutputPath);

				ENGINE_INFO("Generated 3D scene asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}