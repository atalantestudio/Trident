// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/Sampler.hpp"

namespace Trident {
	struct DescriptorBuffer {
		ref<BaseBuffer> buffer;
		uint64 offset;
		uint64 size;
	};

	struct DescriptorImage {
		ref<ImageView> imageView;
		ImageLayout imageLayout;
		ref<Sampler> sampler;
	};

	class DescriptorSet {};

	struct WriteDescriptorSet {
		ref<DescriptorSet> descriptorSet;
		uint32 bindingIndex;

		// TODO: Wrap in an union. Either fill the descriptor buffers or the descriptor images.
		// TODO(graphics/descriptor-buffer): Deprecate (use WriteDescriptorSet::descriptorBufferViews instead).
		view<DescriptorBuffer> descriptorBuffers;

		view<ref<BaseBufferView>> descriptorBufferViews;
		view<DescriptorImage> descriptorImages;
	};
}