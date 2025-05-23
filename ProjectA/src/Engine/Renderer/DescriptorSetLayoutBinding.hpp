#pragma once

#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		ShaderStage stageFlags;
		std::vector<ref<Buffer>> buffers;

		// Applies to every buffer of the descriptor.
		uint64 bufferOffset = 0;

		std::vector<ref<ImageView>> imageViews;
		std::vector<ref<Sampler>> samplers;
	};
}