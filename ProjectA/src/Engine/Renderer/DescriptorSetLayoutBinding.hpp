#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"
#include "Engine/Renderer/ImageView.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		ShaderStage stageFlags;
		chain<ref<Buffer>> buffers;
		chain<ref<ImageView>> imageViews;
		chain<ref<Sampler>> samplers;
	};
}