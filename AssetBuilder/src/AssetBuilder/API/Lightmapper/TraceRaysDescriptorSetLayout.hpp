// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Lightmapper::createTraceRaysDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Argument
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Material
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Emissive triangle index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// G-Buffer position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// G-Buffer smooth position
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// G-Buffer triangle normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// G-Buffer vertex normal
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Intermediate lightmap atlas
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return device->createDescriptorSetLayout(bindings);
	}
}