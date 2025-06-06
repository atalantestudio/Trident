#pragma once

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/Sampler.hpp"
#include "Engine/API/Rendering/ShaderStage.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;

		// TODO: Use as flag.
		ShaderStage stageFlags;

		std::vector<ref<Buffer>> buffers;

		// Applies to every buffer of the descriptor.
		uint64 bufferOffset = 0;

		std::vector<ref<ImageView>> imageViews;
		std::vector<ref<Sampler>> samplers;
	};
}