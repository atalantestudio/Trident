// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	LightmapNode* MapBuilder::insertLightmapNode(const Lightmap& lightmap, LightmapNode* node) {
		if (!node->leaf()) {
			LightmapNode* newNode = insertLightmapNode(lightmap, node->children[0]);

			if (newNode != nullptr) {
				return newNode;
			}

			return insertLightmapNode(lightmap, node->children[1]);
		}

		if (node->index < max<uint32>()) {
			return nullptr;
		}

		if (
			node->rectangle.width < lightmap.size.x ||
			node->rectangle.height < lightmap.size.y
		) {
			return nullptr;
		}

		if (
			node->rectangle.width == lightmap.size.x &&
			node->rectangle.height == lightmap.size.y
		) {
			return node;
		}

		node->children[0] = new LightmapNode();
		node->children[0]->rectangle = node->rectangle;

		node->children[1] = new LightmapNode();
		node->children[1]->rectangle = node->rectangle;

		const Extent2D remainingSize = {
			node->rectangle.width - lightmap.size.x,
			node->rectangle.height - lightmap.size.y,
		};

		// Split towards the axis with the largest available space.
		if (remainingSize.x > remainingSize.y) {
			node->children[0]->rectangle.width = lightmap.size.x;

			node->children[1]->rectangle.x += lightmap.size.x;
			node->children[1]->rectangle.width = remainingSize.x;
		}
		else {
			node->children[0]->rectangle.height = lightmap.size.y;

			node->children[1]->rectangle.y += lightmap.size.y;
			node->children[1]->rectangle.height = remainingSize.y;
		}

		return insertLightmapNode(lightmap, node->children[0]);
	}

	void MapBuilder::createLightmaps(const BuildScene3D& scene, BuildStaticSceneGeometry3D& staticGeometry) {
		lightmaps = sequence<Lightmap>(lightmapCount);
		lightmapMaterials = sequence<LightmapMaterial>(lightmapCount);

		lightmapLookups.reserve(lightmapCount);

		uint32 lightmapCoordinateOffset = 0;
		uint32 lightmapIndex = 0;
		uint32 geometryIndex = 0;

		while (geometryIndex < geometries.size()) {
			const IntermediateGeometry& intermediateGeometry = geometries[geometryIndex];

			if (intermediateGeometry.assetIdentifier != max<uint32>() || !intermediateGeometry.staticUsage) {
				geometryIndex += 1;

				continue;
			}

			const BuildGeometry& geometry = scene.geometries[intermediateGeometry.filteredIndex];
			const uint32 geometryTriangleOffset = geometry.indexRange.offset / 3;

			uint32 faceIndex = 0;

			while (faceIndex < intermediateGeometry.faceIndices.size()) {
				const Face& face = faces[intermediateGeometry.faceIndices[faceIndex]];

				Lightmap& lightmap = lightmaps[lightmapIndex];
				lightmap.geometryIndex = geometryIndex;
				lightmap.materialIndex = lightmapIndex;
				lightmap.triangleRange.offset = geometryTriangleOffset + face.triangleOffset;
				lightmap.triangleRange.count = face.getTriangleCount();
				lightmap.vertexRange.offset = face.vertexOffset;
				lightmap.coordinateOffset = lightmapCoordinateOffset;
				lightmap.coordinateCount = face.getUniqueIndexCount();
				lightmap.minCoordinate = face.boundingBox.min;
				lightmap.maxCoordinate = face.boundingBox.max;
				lightmap.vertexRange.count = lightmap.coordinateCount;
				lightmap.scale = lightmap.maxCoordinate - lightmap.minCoordinate;
				lightmap.luxelScale = face.lightmap.luxelScale;

				LightmapMaterial& lightmapMaterial = lightmapMaterials[lightmapIndex];
				lightmapMaterial.emissiveColor = face.lightmap.emissiveColor;
				lightmapMaterial.emissiveIntensity = face.lightmap.emissiveIntensity;
				lightmapMaterial.albedo = face.lightmap.albedo;

				lightmapCoordinateOffset += lightmap.coordinateCount;
				lightmapIndex += 1;
				faceIndex += 1;
			}

			geometryIndex += 1;
		}

		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

			if (materialShader.asset.geometryType == GeometryType::DYNAMIC) {
				materialShaderIndex += 1;

				continue;
			}

			uint32 materialIndex = 0;

			while (materialIndex < materialShader.materials.size()) {
				const IntermediateMaterial& material = materialShader.materials[materialIndex];

				uint32 materialGeometryIndex = 0;

				while (materialGeometryIndex < material.geometries.size()) {
					if (materialGeometryIndex == material.bakedGeometryIndex) {
						materialGeometryIndex += 1;

						continue;
					}

					const IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];
					const IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

					uint32 instanceIndex = 0;

					while (instanceIndex < materialGeometry.instances.size()) {
						const BuildInstance& instance = materialGeometry.instances[instanceIndex];

						Lightmap& lightmap = lightmaps[lightmapIndex];
						lightmap.geometryIndex = materialGeometry.geometryIndex;
						lightmap.materialIndex = lightmapIndex;
						lightmap.triangleRange.offset = 0;
						lightmap.triangleRange.count = static_cast<uint32>(geometry.indices.count() / 3);
						lightmap.vertexRange.offset = 0;
						lightmap.coordinateOffset = lightmapCoordinateOffset;
						lightmap.coordinateCount = static_cast<uint32>(geometry.vertices.count());
						lightmap.minCoordinate = geometry.boundingBox.min;
						lightmap.maxCoordinate = geometry.boundingBox.max;
						lightmap.vertexRange.count = lightmap.coordinateCount;
						lightmap.scale = instance.lightmapScale;
						lightmap.luxelScale = instance.lightmap.luxelScale;

						LightmapMaterial& lightmapMaterial = lightmapMaterials[lightmapIndex];
						lightmapMaterial.emissiveColor = instance.lightmap.emissiveColor;
						lightmapMaterial.emissiveIntensity = instance.lightmap.emissiveIntensity;
						lightmapMaterial.albedo = instance.lightmap.albedo;

						lightmapCoordinateOffset += lightmap.coordinateCount;
						lightmapIndex += 1;
						instanceIndex += 1;
					}

					materialGeometryIndex += 1;
				}

				materialIndex += 1;
			}

			materialShaderIndex += 1;
		}

		lightmapIndex = 0;

		while (lightmapIndex < lightmaps.count()) {
			Lightmap& lightmap = lightmaps[lightmapIndex];

			const vec2<float32> size = ceil(lightmap.scale / lightmap.luxelScale);

			lightmap.size = vec2<uint32>(size);

			DEBUG_ASSERT(lightmap.size.x > 0 && lightmap.size.y > 0);

			// Pad for bicubic filtering.
			lightmap.size.x += 4;
			lightmap.size.y += 4;

			const IntermediateGeometry& geometry = geometries[lightmap.geometryIndex];
			const view<BuildVertex3D> vertices(&geometry.vertices[lightmap.vertexRange.offset], lightmap.coordinateCount);

			uint32 coordinateIndex = 0;

			while (coordinateIndex < lightmap.coordinateCount) {
				const BuildVertex3D& vertex = vertices[coordinateIndex];
				const float32 packedTextureCoordinate = vertex.tangent[3];
				const vec2<float32> textureCoordinate = unpack<VertexInputAttributeFormat::R16G16_FLOAT, vec2<float32>>(*pointer_cast<const uint32*>(&packedTextureCoordinate));

				staticGeometry.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex] = textureCoordinate * size;

				coordinateIndex += 1;
			}

			LightmapLookup lightmapLookup{};
			lightmapLookup.index = lightmapIndex;
			lightmapLookup.area = lightmap.size.x * lightmap.size.y;

			uint32 lightmapInsertionIndex = 0;

			while (lightmapInsertionIndex < lightmapLookups.size()) {
				if (lightmapLookup.area > lightmapLookups[lightmapInsertionIndex].area) {
					break;
				}

				lightmapInsertionIndex += 1;
			}

			lightmapLookups.insert(lightmapLookups.begin() + lightmapInsertionIndex, lightmapLookup);

			lightmapIndex += 1;
		}
	}

	void MapBuilder::updateLightmapCoordinates(BuildStaticSceneGeometry3D& staticGeometry) {
		static constexpr vec2<float32> inverseAtlasExtent = 1.0f / vec2<float32>(LIGHTMAP_ATLAS_EXTENT);

		LightmapNode rootNode{};
		rootNode.rectangle = {0, 0, LIGHTMAP_ATLAS_EXTENT.x, LIGHTMAP_ATLAS_EXTENT.y};

		for (const LightmapLookup lightmapLookup : lightmapLookups) {
			const Lightmap& lightmap = lightmaps[lightmapLookup.index];

			LightmapNode* node = insertLightmapNode(lightmap, &rootNode);

			ENGINE_ASSERT(node != nullptr, "There's not enough remaining space in the lightmap atlas to insert the node.");

			node->index = lightmapLookup.index;

			// TODO(lyah/scalar)
			const vec2<float32> lightmapOffset = {
				static_cast<float32>(node->rectangle.x),
				static_cast<float32>(node->rectangle.y),
			};

			uint32 coordinateIndex = 0;

			while (coordinateIndex < lightmap.coordinateCount) {
				vec2<float32>& coordinate = staticGeometry.lightmapCoordinates[lightmap.coordinateOffset + coordinateIndex];
				coordinate = (coordinate + lightmapOffset) * inverseAtlasExtent;

				coordinateIndex += 1;
			}
		}
	}
}