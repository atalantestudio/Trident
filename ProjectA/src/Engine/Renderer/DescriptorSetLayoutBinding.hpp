#pragma once

#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/Sampler.hpp"
#include "Engine/API/Rendering/ShaderStage.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;

		// TODO: Replace by flag enum?
		ShaderStage stageFlags;

		std::vector<ref<Buffer>> buffers;

		// Applies to every buffer of the descriptor.
		uint64 bufferOffset = 0;

		std::vector<ref<ImageView>> imageViews;
		std::vector<ref<Sampler>> samplers;
	};
}