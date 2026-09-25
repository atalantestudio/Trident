// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::placeStaticProp(const MeshDescriptor& descriptor) {
		const uint32 materialShaderIndex = descriptor.material >> 16;
		const uint32 materialIndex = descriptor.material & 0xff;

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		ENGINE_ASSERT(materialShader.asset.geometryType == GeometryType::STATIC, "Cannot place a static prop with a dynamic material.");

		IntermediateMaterial& material = materialShader.materials[materialIndex];

		const uint32 materialGeometryIndex = findMaterialGeometryIndex(material, descriptor.geometryIndex);

		if (materialGeometryIndex >= material.geometries.size()) {
			IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
			geometry.staticUsage = true;

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = descriptor.geometryIndex;

			material.geometries.emplace_back(materialGeometry);

			staticGeometryPropVertexCount += geometry.vertices.count();
			staticGeometryPropIndexCount += geometry.indices.count();
			materialGeometryCount += 1;
		}

		IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
		geometry.sharedInstanceCount += 1;
		geometry.boundingBox.max = {min<float32>(), min<float32>()};
		geometry.boundingBox.min = {max<float32>(), max<float32>()};

		const BuildInstance instance = {
			.transform = descriptor.transform,
			.lightmap = descriptor.lightmap,
		};

		IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];
		materialGeometry.instances.emplace_back(instance);

		uint32 vertexIndex = 0;

		while (vertexIndex < geometry.vertices.count()) {
			BuildVertex3D& vertex = geometry.vertices[vertexIndex];

			const uint32 textureCoordinate = pack<VertexInputAttributeFormat::R16G16_FLOAT, uint32>(vertex.textureCoordinate);

			vertex.tangent = {vertex.tangent[0], vertex.tangent[1], vertex.tangent[2], *pointer_cast<const float32*>(&textureCoordinate)};

			geometry.boundingBox.max.x = max(geometry.boundingBox.max.x, vertex.textureCoordinate.x);
			geometry.boundingBox.max.y = max(geometry.boundingBox.max.y, vertex.textureCoordinate.y);
			geometry.boundingBox.min.x = min(geometry.boundingBox.min.x, vertex.textureCoordinate.x);
			geometry.boundingBox.min.y = min(geometry.boundingBox.min.y, vertex.textureCoordinate.y);

			vertexIndex += 1;
		}

		staticInstanceCount += 1;
		lightmapCount += 1;
	}
}