// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createMaterialBaseDescriptorSetLayout() const {
		const DescriptorSetLayoutBinding bindings[] = {
			// Camera
			{
				.descriptorType = DescriptorType::UNIFORM_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Material coordinate
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Visibility
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex position component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex normal component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex texture coordinate component
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex position index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex normal index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Vertex texture coordinate index
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Cluster
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Instance
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Geometry
			{
				.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},

			// Output
			{
				.descriptorType = DescriptorType::READ_WRITE_STORAGE_IMAGE,
				.descriptorCount = 1,
				.stageFlags = ShaderStage::COMPUTE,
			},
		};

		return createDescriptorSetLayout({
			.bindings = bindings,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material base",
			#endif
		});
	}
}