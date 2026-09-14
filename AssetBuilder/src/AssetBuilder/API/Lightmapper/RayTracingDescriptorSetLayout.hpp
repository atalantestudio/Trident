// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Lightmapper::createRayTracingDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// TLAS
			{
				.descriptorType = DescriptorType::ACCELERATION_STRUCTURE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
			},

			// Argument
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
			},

			// Lightmap coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
			},

			// Index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_CLOSEST_HIT,
			},

			// Emissive triangle index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
			},

			// Emissive triangle
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
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
				.stageFlags = ShaderStage::RAY_GENERATION,
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
				.stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
			},

			// G-Buffer smooth position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
			},

			// G-Buffer triangle normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
			},

			// G-Buffer vertex normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION | ShaderStage::RAY_CLOSEST_HIT,
			},

			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::RAY_GENERATION,
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