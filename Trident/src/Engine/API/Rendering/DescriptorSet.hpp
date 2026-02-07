// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/ImageView.hpp"
#include "Engine/API/Rendering/Sampler.hpp"

namespace Trident {
	struct DescriptorBuffer {
		ref<Buffer> buffer;
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

		// NOTE: Either fill the descriptor buffers or the descriptor images.
		view<DescriptorBuffer> descriptorBuffers;
		view<DescriptorImage> descriptorImages;
	};
}