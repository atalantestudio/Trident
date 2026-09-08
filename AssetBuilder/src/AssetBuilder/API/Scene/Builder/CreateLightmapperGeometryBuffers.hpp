// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<BuildVertex3D>& vertices, sequence<uint32>& indices, sequence<LightmapMaterial>& materials, sequence<uint32>& emissiveTriangleIndices) {
		vertices = sequence<BuildVertex3D>(lightmapVertexCount);
		indices = sequence<uint32>(lightmapIndexCount);
		materials = sequence<LightmapMaterial>(lightmapMaterials.size());

		uint32 vertexIndex = 0;

		while (vertexIndex < scene.staticGeometry.bakedVertexCount) {
			BuildVertex3D& vertex = vertices[vertexIndex];
			vertex = scene.vertices[vertexIndex];
			vertex.textureCoordinate = scene.staticGeometry.lightmapCoordinates[vertexIndex];

			vertexIndex += 1;
		}

		std::copy_n(&scene.indices[0], scene.staticGeometry.bakedIndexCount, &indices[0]);

		uint32 vertexOffset = scene.staticGeometry.bakedVertexCount;
		uint32 indexOffset = scene.staticGeometry.bakedIndexCount;
		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

			if (materialShader.asset.geometryType != GeometryType::STATIC) {
				materialShaderIndex += 1;

				continue;
			}

			for (const IntermediateMaterial& material : materialShader.materials) {
				for (const IntermediateMaterialGeometry& materialGeometry : material.geometries) {
					const IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

					if (geometry.assetIdentifier == max<uint32>()) {
						continue;
					}

					for (const BuildTransform3D& instance : materialGeometry.instances) {
						const quat<float32> orientation = normalized(
							quat<float32>::axisAngle(UP, instance.rotation[1]) *
							quat<float32>::axisAngle(RIGHT, instance.rotation[0]) *
							quat<float32>::axisAngle(FORWARD, instance.rotation[2])
						);
						const quat<float32> inverseOrientation = inverse(orientation);
						const mat4x4<float32> worldTransform = (
							mat4x4<float32>::scaling({instance.scale, instance.scale, instance.scale}) *
							mat4x4<float32>(orientation) *
							mat4x4<float32>::translation(instance.translation)
						);

						uint32 vertexIndex = 0;

						while (vertexIndex < geometry.vertices.count()) {
							const BuildVertex3D& geometryVertex = geometry.vertices[vertexIndex];
							const float32 textureCoordinate = geometryVertex.tangent[3];

							// TODO(lyah/scalar)
							BuildVertex3D& vertex = vertices[vertexOffset + vertexIndex];
							vertex.position = svec3<float32>(multiplyTransform(vec3<float32>(geometryVertex.position), worldTransform));
							vertex.normal = svec3<float32>(normalized(vec3<float32>(geometryVertex.normal) * inverseOrientation));

							// TODO(asset-builder/lightmapper)
							// vertex.textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, svec2<float32>>(*pointer_cast<const uint32*>(&textureCoordinate));
							vertex.textureCoordinate = scene.staticGeometry.lightmapCoordinates[vertexOffset + vertexIndex];

							vertexIndex += 1;
						}

						uint32 indexIndex = 0;

						while (indexIndex < geometry.indices.count()) {
							indices[indexOffset + indexIndex] = vertexOffset + geometry.indices[indexIndex];

							indexIndex += 1;
						}

						vertexOffset += vertexIndex;
						indexOffset += indexIndex;
					}
				}
			}

			materialShaderIndex += 1;
		}

		std::vector<uint32> emissiveTriangleIndices_;

		{
			uint32 lightmapIndex = 0;
			uint64 vertexOffset = 0;

			while (lightmapIndex < lightmaps.size()) {
				const Lightmap& lightmap = lightmaps[lightmapIndex];

				uint64 vertexIndex = 0;

				while (vertexIndex < lightmap.vertexRange.count) {
					vertices[vertexOffset + vertexIndex].tangent = {0.0f, 0.0f, 0.0f, static_cast<float32>(lightmapIndex)};

					vertexIndex += 1;
				}

				if (lightmapMaterials[lightmapIndex].emissiveIntensity > 0.0f) {
					const uint32 geometryTriangleOffset = scene.geometries[lightmap.geometryIndex].indexRange.offset / 3;

					uint32 triangleIndex = 0;

					while (triangleIndex < lightmap.triangleRange.count) {
						emissiveTriangleIndices_.emplace_back(geometryTriangleOffset + lightmap.triangleRange.offset + triangleIndex);

						triangleIndex += 1;
					}
				}

				vertexOffset += lightmap.vertexRange.count;
				lightmapIndex += 1;
			}
		}

		emissiveTriangleIndices = sequence<uint32>(emissiveTriangleIndices_.size());

		std::copy(lightmapMaterials.begin(), lightmapMaterials.end(), &materials[0]);
		std::copy(emissiveTriangleIndices_.begin(), emissiveTriangleIndices_.end(), &emissiveTriangleIndices[0]);
	}
}