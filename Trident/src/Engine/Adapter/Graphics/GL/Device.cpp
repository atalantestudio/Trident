// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/Buffer.hpp"
#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Device::Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice) :
		Trident::Device(adapter, physicalDevice)
	{}

	AcquireNextImageStatus GL::acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Trident::Semaphore* semaphore, uint32& imageIndex) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		imageIndex = 0;

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<Trident::DescriptorSet>> GL::allocateDescriptorSets(Trident::Device* device, view<ref<Trident::DescriptorSetLayout>> descriptorSetLayouts) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		sequence<ref<Trident::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count(); descriptorSetIndex += 1) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			ref<GL::DescriptorSet> descriptorSet = allocateRef<GL::DescriptorSet>();
			descriptorSet->bindings = sequence<GL::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (uint32 descriptorSetBindingIndex = 0; descriptorSetBindingIndex < descriptorSet->bindings.count(); descriptorSetBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetBindingIndex];

				GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[descriptorSetBindingIndex];
				descriptorSetBinding.descriptorType = descriptorSetLayoutBinding.descriptorType;

				if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
					descriptorSetBinding.descriptorBufferViews = sequence<BaseBufferView>(descriptorSetLayoutBinding.descriptorCount);
				}
				else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
					descriptorSetBinding.descriptorImages = sequence<DescriptorImage>(descriptorSetLayoutBinding.descriptorCount);
				}
				else {
					ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
				}
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;
		}

		return descriptorSets;
	}

	ref<Trident::Device> GL::createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice, view<DeviceExtensionName> enabledExtensions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<GL::Device> device = allocateRef<GL::Device>(adapter, physicalDevice);
		device->queues[0] = createQueue(device, 0, QueueFamilyCapability::GRAPHICS);
		device->queues[1] = createQueue(device, 1, QueueFamilyCapability::COMPUTE);
		device->queues[2] = createQueue(device, 2, QueueFamilyCapability::TRANSFER);

		return device;
	}

	void GL::destroyDevice(const Trident::Device* device) {}


	void GL::resetFence(const Trident::Device* device, const Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const GL::Fence* _fence = static_cast<const GL::Fence*>(fence);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			glDeleteSync(_fence->sync);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void GL::updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<GL::DescriptorSet> descriptorSet = ref_cast<GL::DescriptorSet>(writeDescriptorSet.descriptorSet);

			GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				std::copy(writeDescriptorSet.descriptorBufferViews.begin(), writeDescriptorSet.descriptorBufferViews.end(), descriptorSetBinding.descriptorBufferViews.begin());

				continue;
			}

			if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				DEBUG_ASSERT(writeDescriptorSet.descriptorImages.count() > 0);

				std::copy(writeDescriptorSet.descriptorImages.begin(), writeDescriptorSet.descriptorImages.end(), descriptorSetBinding.descriptorImages.begin());

				continue;
			}

			ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
		}
	}

	void GL::waitForFence(const Trident::Device* device, Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			GL::Fence* _fence = static_cast<GL::Fence*>(fence);
			GLenum status;

			_fence->sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

			ADAPTER_ASSERT(_fence->sync != 0, "Failed to create fence.");

			status = glClientWaitSync(_fence->sync, GL_SYNC_FLUSH_COMMANDS_BIT, max<GLuint64>());

			DEBUG_ASSERT(status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void GL::waitIdle(const Trident::Device* device) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		glFinish();
	}
}