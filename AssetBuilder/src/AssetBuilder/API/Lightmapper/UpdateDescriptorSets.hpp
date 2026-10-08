// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	void Lightmapper::updateDescriptorSets() const {
		const BaseBufferView bufferViews[] = {
			argumentBuffer->createView<Arguments>(0, argumentBuffer->getSize()),
			vertexBuffer->createView<LightmapVertex>(0, vertexBuffer->getSize()),
			lightmapCoordinateBuffer->createView<vec2<float32>>(0, lightmapCoordinateBuffer->getSize()),
			indexBuffer->createView<uint32>(0, indexBuffer->getSize()),
			emissiveTriangleIndexBuffer->createView<uint32>(0, emissiveTriangleIndexBuffer->getSize()),
			triangleBuffer->createView<BuildTriangle3D>(0, triangleBuffer->getSize()),
			instanceBuffer->createView<LightmapInstance>(0, instanceBuffer->getSize()),
			materialBuffer->createView<LightmapMaterial>(0, materialBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[] = {
			{
				.imageView = environmentMapImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = environmentMapSampler,
			},
			{
				.imageView = context.gBufferPositionImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferSmoothPositionImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferTriangleNormalImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.gBufferVertexNormalImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.intermediateLightmapAtlasImageView,
				.imageLayout = ImageLayout::GENERAL,
				.sampler = nullptr,
			},
			{
				.imageView = context.intermediateLightmapAtlasImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = intermediateLightmapAtlasSampler,
			},
		};

		const WriteDescriptorSet writeDescriptorSets[] = {
			/// G-Buffer

			// Vertex
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 0,
				.descriptorBufferViews = bufferViews[1],
			},

			// Lightmap coordinate
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 1,
				.descriptorBufferViews = bufferViews[2],
			},

			// Index
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 2,
				.descriptorBufferViews = bufferViews[3],
			},

			// Triangle
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 3,
				.descriptorBufferViews = bufferViews[5],
			},

			// Instance
			{
				.descriptorSet = descriptorSets[0],
				.bindingIndex = 4,
				.descriptorBufferViews = bufferViews[6],
			},

			/// Ray tracing

			// TLAS
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 0,
				.accelerationStructures = tlas,
			},

			// Argument
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 1,
				.descriptorBufferViews = bufferViews[0],
			},

			// Lightmap coordinate
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 2,
				.descriptorBufferViews = bufferViews[2],
			},

			// Index
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 3,
				.descriptorBufferViews = bufferViews[3],
			},

			// Emissive triangle index
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 4,
				.descriptorBufferViews = bufferViews[4],
			},

			// Emissive triangle
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 5,
				.descriptorBufferViews = bufferViews[5],
			},

			// Instance
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 6,
				.descriptorBufferViews = bufferViews[6],
			},

			// Material
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 7,
				.descriptorBufferViews = bufferViews[7],
			},

			// Environment map
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 8,
				.descriptorImages = descriptorImages[0],
			},

			// G-Buffer position
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 9,
				.descriptorImages = descriptorImages[1],
			},

			// G-Buffer smooth position
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 10,
				.descriptorImages = descriptorImages[2],
			},

			// G-Buffer triangle normal
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 11,
				.descriptorImages = descriptorImages[3],
			},

			// G-Buffer vertex normal
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 12,
				.descriptorImages = descriptorImages[4],
			},

			// Intermediate lightmap atlas
			{
				.descriptorSet = descriptorSets[1],
				.bindingIndex = 13,
				.descriptorImages = descriptorImages[5],
			},

			/// Post processing

			// Intermediate lightmap atlas
			{
				.descriptorSet = descriptorSets[2],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[5],
			},

			/// Dilation

			// Intermediate lightmap atlas
			{
				.descriptorSet = descriptorSets[3],
				.bindingIndex = 0,
				.descriptorImages = descriptorImages[6],
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);
	}
}