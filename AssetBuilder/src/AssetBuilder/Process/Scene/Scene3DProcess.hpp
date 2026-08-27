// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Process/Process.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "AssetBuilder/API/Scene/Scene3D.hpp"
#include "AssetBuilder/Process/Image/ImageProcess.hpp"

namespace Trident {
	class Scene3DProcess : public Process {
		private:
			static ByteStream<StreamOperation::WRITE> encodeSceneAsset(const BuildScene3D& scene, uint32 identifier) {
				const uint64 streamSize = calculateStreamSize(scene);

				ByteStream<StreamOperation::WRITE> stream(streamSize, Endianness::LITTLE_ENDIAN);
				stream.write<AssetSignature>(AssetSignature::SCENE);
				stream.write<uint32>(identifier);

				// TODO(asset-builder/scene): Map building WIP.
				stream.write<VersionNumber>({0, 1, 0});

				stream.write<float32>(scene.skyColor.x);
				stream.write<float32>(scene.skyColor.y);
				stream.write<float32>(scene.skyColor.z);
				stream.write<uint32>(scene.skyboxAssetIdentifier);
				stream.write<uint8>(sizeof(uint32));
				stream.write<uint32>(scene.vertexCount);
				stream.write<uint32>(scene.staticGeometry.vertexCount);
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.bakedVertices.count()));
				stream.write<uint32>(scene.triangleCount);
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.bakedIndices.count() / 3));
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.lightmapCoordinates.count()));
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.instances.count()));
				stream.write<uint32>(static_cast<uint32>(scene.geometries.count()));
				stream.write<uint32>(static_cast<uint32>(scene.materialGeometries.count()));
				stream.write<uint32>(static_cast<uint32>(scene.materials.count()));
				stream.write<uint32>(static_cast<uint32>(scene.staticGeometry.materialShaders.count()));
				stream.write<uint32>(static_cast<uint32>(scene.dynamicGeometry.materialShaders.count()));
				stream.write<uint32>(scene.dynamicGeometry.entityCount);
				stream.write<float32>(scene.sunDirection.x);
				stream.write<float32>(scene.sunDirection.y);
				stream.write<float32>(scene.sunDirection.z);
				stream.write<float32>(scene.sunColor.x);
				stream.write<float32>(scene.sunColor.y);
				stream.write<float32>(scene.sunColor.z);
				stream.write<uint32>(static_cast<uint32>(scene.dynamicGeometry.pointLights.count()));
				stream.write<uint32>(scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier);

				for (const BuildVertex3D& vertex : scene.staticGeometry.bakedVertices) {
					stream.write<float32>(vertex.position.x);
					stream.write<float32>(vertex.position.y);
					stream.write<float32>(vertex.position.z);
					stream.write<uint32>(pack<VertexInputAttributeFormat::R10G10B10A2_UNORM, uint32>(vertex.normal));
					stream.write<uint32>(pack<VertexInputAttributeFormat::R10G10B10A2_UNORM, uint32>(vertex.tangent));
					stream.write<uint32>(pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(vertex.textureCoordinate));
				}

				for (const uint32 index : scene.staticGeometry.bakedIndices) {
					stream.write<uint32>(index);
				}

				for (const svec2<float32> lightmapCoordinate : scene.staticGeometry.lightmapCoordinates) {
					stream.write<uint32>(pack<VertexInputAttributeFormat::R16G16_UNORM, uint32>(lightmapCoordinate));
				}

				for (const BuildTransform3D& instance : scene.staticGeometry.instances) {
					stream.write<float32>(instance.translation.x);
					stream.write<float32>(instance.translation.y);
					stream.write<float32>(instance.translation.z);
					stream.write<float32>(instance.rotation.x);
					stream.write<float32>(instance.rotation.y);
					stream.write<float32>(instance.rotation.z);
					stream.write<float32>(instance.scale);
				}

				for (const BuildGeometry geometry : scene.geometries) {
					stream.write<range<uint32, uint32>>(geometry.indexRange);
					stream.write<uint32>(geometry.assetIdentifier);
				}

				for (const BuildMaterialGeometry materialGeometry : scene.materialGeometries) {
					stream.write<range<uint32, uint32>>(materialGeometry.instanceRange);
					stream.write<uint32>(materialGeometry.geometryIndex);
				}

				for (const BuildMaterial material : scene.materials) {
					stream.write<uint32>(material.assetIdentifier);
					stream.write<uint32>(material.geometryCount);
				}

				for (const BuildMaterialShader& materialShader : scene.staticGeometry.materialShaders) {
					encodeMaterialShader(stream, materialShader);
				}

				for (const BuildMaterialShader& materialShader : scene.dynamicGeometry.materialShaders) {
					encodeMaterialShader(stream, materialShader);
				}

				for (const PointLight& pointLight : scene.dynamicGeometry.pointLights) {
					stream.write<float32>(pointLight.color.x);
					stream.write<float32>(pointLight.color.y);
					stream.write<float32>(pointLight.color.z);
					stream.write<float32>(pointLight.intensity);
					stream.write<float32>(pointLight.position.x);
					stream.write<float32>(pointLight.position.y);
					stream.write<float32>(pointLight.position.z);
				}

				for (const EntityComponentMapping<GeometryComponent> geometryMapping : scene.dynamicGeometry.geometryMappings) {
					stream.write<Entity>(geometryMapping.entity);
					stream.write<GeometryComponent>(geometryMapping.component);
				}

				for (const EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : scene.dynamicGeometry.rigidBodyMappings) {
					const RigidBody3DComponent& component = rigidBodyMapping.component;

					stream.write<Entity>(rigidBodyMapping.entity);
					stream.write<svec3<float32>>(svec3<float32>(component.position));
					stream.write<svec3<float32>>(svec3<float32>(component.rotation));
					stream.write<svec3<float32>>(svec3<float32>(component.linearVelocity));
					stream.write<svec3<float32>>(svec3<float32>(component.angularVelocity));
					stream.write<float32>(component.scale);
					stream.write<float32>(component.inverseMass);
					stream.write<svec3<float32>>(svec3<float32>(component.inverseInertia[0]));
					stream.write<svec3<float32>>(svec3<float32>(component.inverseInertia[1]));
					stream.write<svec3<float32>>(svec3<float32>(component.inverseInertia[2]));
					stream.write<float32>(component.friction);
					stream.write<float32>(component.restitution);
				}

				return stream;
			}

			static void encodeMaterialShader(ByteStream<StreamOperation::WRITE>& stream, const BuildMaterialShader& materialShader) {
				stream.write<AssetSignature>(AssetSignature::SHADER_MODULE);
				stream.write<uint32>(materialShader.asset.identifier);
				stream.write<VersionNumber>({0, 0, 0});
				stream.write<uint32>(static_cast<uint32>(materialShader.asset.name.count()));
				stream.write<ShaderStage>(materialShader.asset.stageFlags);
				stream.write<uint32>(static_cast<uint32>(materialShader.asset.entryPoint.count()));
				stream.write<uint32>(static_cast<uint32>(materialShader.asset.bindings.count()));
				stream.write<uint32>(static_cast<uint32>(materialShader.asset.descriptorSetBindingRanges.count()));

				for (const char8 character : materialShader.asset.name) {
					stream.write(character);
				}

				for (const char8 character : materialShader.asset.entryPoint) {
					stream.write(character);
				}

				for (const DescriptorSetLayoutBinding& binding : materialShader.asset.bindings) {
					stream.write<DescriptorType>(binding.descriptorType);
					stream.write<uint32>(binding.descriptorCount);
					stream.write<ShaderStage>(binding.stageFlags);
				}

				for (const range<uint32, uint32> bindingRange : materialShader.asset.descriptorSetBindingRanges) {
					stream.write<uint32>(bindingRange.count);
				}

				stream.write<range<uint32, uint32>>(materialShader.materialRange);
			}

			static uint64 calculateStreamSize(const BuildScene3D& scene) {
				uint64 size = (
					sizeof(AssetHeader) +
					sizeof(uint8) +
					sizeof(uint32) * 16 +
					sizeof(svec3<float32>) * 3 +
					scene.staticGeometry.bakedVertices.count() * sizeof(Vertex3D) +
					scene.staticGeometry.bakedIndices.count() * sizeof(uint32) +
					scene.staticGeometry.lightmapCoordinates.count() * sizeof(uint32) +
					scene.staticGeometry.instances.count() * sizeof(BuildTransform3D) +
					scene.geometries.count() * sizeof(BuildGeometry) +
					scene.materialGeometries.count() * sizeof(BuildMaterialGeometry) +
					scene.materials.count() * sizeof(BuildMaterial) +
					scene.dynamicGeometry.pointLights.count() * (sizeof(svec3<float32>) * 2 + sizeof(float32)) +
					scene.dynamicGeometry.geometryMappings.count() * sizeof(EntityComponentMapping<GeometryComponent>) +
					scene.dynamicGeometry.rigidBodyMappings.count() * (sizeof(Entity) + sizeof(svec3<float32>) * 7 + sizeof(float32) * 4)
				);

				for (const BuildMaterialShader& materialShader : scene.staticGeometry.materialShaders) {
					size += calculateMaterialShaderSize(materialShader);
				}

				for (const BuildMaterialShader& materialShader : scene.dynamicGeometry.materialShaders) {
					size += calculateMaterialShaderSize(materialShader);
				}

				return size;
			}

			static uint64 calculateMaterialShaderSize(const BuildMaterialShader& materialShader) {
				return (
					sizeof(AssetHeader) +
					sizeof(uint32) * 4 +
					sizeof(ShaderStage) +
					materialShader.asset.name.count() +
					materialShader.asset.entryPoint.count() +
					materialShader.asset.bindings.count() * (sizeof(DescriptorType) + sizeof(uint32) + sizeof(ShaderStage)) +
					materialShader.asset.descriptorSetBindingRanges.count() * sizeof(uint32) +
					sizeof(range<uint32, uint32>)
				);
			}

		public:
			sint32 main(view<void*> arguments) final override {
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[2]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";
				const view<char8> assetName = getAssetName();

				std::filesystem::path outputPath = assetOutputPath / std::string(assetName);
				outputPath.replace_extension("tsa");

				if (std::filesystem::exists(outputPath)) {
					ENGINE_INFO("Scene asset \"[]\" already exists.", outputPath.filename().string());

					return 0;
				}

				const uint32 assetIdentifier = AssetLibrary::createAssetIdentifier();
				const BuildScene3D scene = createScene(assetLibrary);
				const ByteStream<StreamOperation::WRITE> stream = encodeSceneAsset(scene, assetIdentifier);

				createSceneAsset(scene, assetIdentifier, stream, assetName, rootOutputPath);

				ENGINE_INFO("Generated scene asset \"[]\" (ID: []).", assetName, assetIdentifier);

				return 0;
			}

		protected:
			explicit Scene3DProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			virtual view<char8> getAssetName() const = 0;

			virtual BuildScene3D createScene(AssetBuilderLibrary* assetLibrary) const = 0;

		private:
			void createSceneAsset(const BuildScene3D& scene, uint32 identifier, const ByteStream<StreamOperation::WRITE>& stream, view<char8> assetName, const std::filesystem::path& rootOutputPath) {
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/Scene/";
				const std::string assetOutputPathString = assetOutputPath.string();

				sequence<char8> outputPath(assetOutputPathString.size() + assetName.count() + 5);
				std::copy(assetOutputPathString.begin(), assetOutputPathString.end(), &outputPath[0]);
				std::copy(assetName.begin(), assetName.end(), &outputPath[assetOutputPathString.size()]);
				std::copy_n(".tsa", 4, &outputPath[assetOutputPathString.size() + assetName.count()]);

				Writer writer = Filesystem::openWriter(outputPath);
				writer.write(stream.data, stream.getByteOffset());

				assetLibrary->registerAsset(identifier, assetName);

				if (scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier != max<uint32>()) {
					const ByteStream<StreamOperation::WRITE> stream = ImageProcess::encodeImageAsset(scene.staticGeometry.lightmapAtlasImageAssetDescriptor);

					ImageProcess::createImageAsset(scene.staticGeometry.lightmapAtlasImageAssetDescriptor, stream, assetName, rootOutputPath / "Asset/Image/", assetLibrary);
				}
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}