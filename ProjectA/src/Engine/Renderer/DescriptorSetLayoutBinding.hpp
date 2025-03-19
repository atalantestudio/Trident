#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/DescriptorType.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderStage.hpp"
#include "Engine/Renderer/TextureView.hpp"

namespace ProjectA {
	struct DescriptorSetLayoutBinding {
		DescriptorType descriptorType;
		ShaderStage stageFlags;
		chain<ref<Buffer>> buffers;
		chain<ref<TextureView>> textureViews;

		// NOTE: One sampler per texture view.
		chain<ref<Sampler>> samplers;
	};
}