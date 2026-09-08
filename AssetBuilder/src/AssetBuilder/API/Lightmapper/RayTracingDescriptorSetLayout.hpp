// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Lightmapper::createRayTracingDescriptorSetLayout() const {
		const ShaderStage stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT | ShaderStage::RAY_MISS;

		const DescriptorSetLayoutBinding bindings[] = {
			// TLAS
			{
				.descriptorType = DescriptorType::ACCELERATION_STRUCTURE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION,
				.stageFlags = stageFlags,
			},

			// Argument
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// Vertex
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// Lightmap coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// Index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// Instance
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_CLOSEST_HIT,
			},

			// Material
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION,
				.stageFlags = stageFlags,
			},

			// Emissive triangle index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION,
				.stageFlags = stageFlags,
			},

			// Environment map
			{
				.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
			},

			// G-Buffer position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// G-Buffer smooth position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// G-Buffer triangle normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// G-Buffer vertex normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
				.stageFlags = stageFlags,
			},

			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				// .stageFlags = ShaderStage::RAY_GENERATION,
				.stageFlags = stageFlags,
			},
		};

		return device->createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Ray tracing",
			#endif
		});
	}
}