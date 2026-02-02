#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	ref<DescriptorSetLayout> VisibilityRenderer::createDebugDescriptorSetLayout() const {
		DescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
		cameraBinding.descriptorCount = 1;
		cameraBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding visibilityBinding{};
		visibilityBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		visibilityBinding.descriptorCount = 1;
		visibilityBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding depthStencilBinding{};
		depthStencilBinding.descriptorType = DescriptorType::INPUT_ATTACHMENT;
		depthStencilBinding.descriptorCount = 1;
		depthStencilBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexPositionComponentBinding{};
		vertexPositionComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexPositionComponentBinding.descriptorCount = 1;
		vertexPositionComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexNormalComponentBinding{};
		vertexNormalComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexNormalComponentBinding.descriptorCount = 1;
		vertexNormalComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexTextureCoordinateComponentBinding{};
		vertexTextureCoordinateComponentBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexTextureCoordinateComponentBinding.descriptorCount = 1;
		vertexTextureCoordinateComponentBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexPositionIndexBinding{};
		vertexPositionIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexPositionIndexBinding.descriptorCount = 1;
		vertexPositionIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexNormalIndexBinding{};
		vertexNormalIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexNormalIndexBinding.descriptorCount = 1;
		vertexNormalIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		DescriptorSetLayoutBinding vertexTextureCoordinateIndexBinding{};
		vertexTextureCoordinateIndexBinding.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER;
		vertexTextureCoordinateIndexBinding.descriptorCount = 1;
		vertexTextureCoordinateIndexBinding.shaderStages = ShaderStage::FRAGMENT;

		return createDescriptorSetLayout({
			cameraBinding,
			visibilityBinding,
			depthStencilBinding,
			vertexPositionComponentBinding,
			vertexNormalComponentBinding,
			vertexTextureCoordinateComponentBinding,
			vertexPositionIndexBinding,
			vertexNormalIndexBinding,
			vertexTextureCoordinateIndexBinding,
		});
	}
}