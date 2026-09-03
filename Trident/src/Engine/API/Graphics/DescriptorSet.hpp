// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/Sampler.hpp"

namespace Trident {
	struct DescriptorImage {
		ref<ImageView> imageView;
		ImageLayout imageLayout;
		ref<Sampler> sampler;
	};

	class DescriptorSet {};

	struct WriteDescriptorSet {
		ref<DescriptorSet> descriptorSet;
		uint32 bindingIndex;

		// TODO(graphics/write-descriptor-set): Wrap in an union.
		view<ref<AccelerationStructure>> accelerationStructures;
		view<BaseBufferView> descriptorBufferViews;
		view<DescriptorImage> descriptorImages;
	};
}