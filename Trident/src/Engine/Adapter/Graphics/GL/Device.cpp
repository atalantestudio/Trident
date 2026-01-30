// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/GL/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/GL/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Fence.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	GL::Device::Device(const GL::Instance& instance) :
		physicalDevice(createRef<GL::PhysicalDevice>()),
		queue(createRef<GL::Queue>(instance, *this))
	{}

	AcquireNextImageStatus GL::Device::acquireNextImage(ref<Trident::Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const {
		// The buffering is handled by WGL.
		imageIndex = 0;

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<Trident::DescriptorSet>> GL::Device::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		sequence<ref<Trident::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count(); descriptorSetIndex += 1) {
			const ref<DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];
			const ref<GL::DescriptorSet> descriptorSet = createRef<GL::DescriptorSet>();

			descriptorSet->bindings = sequence<GL::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (uint32 descriptorSetBindingIndex = 0; descriptorSetBindingIndex < descriptorSet->bindings.count(); descriptorSetBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetBindingIndex];

				GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[descriptorSetBindingIndex];
				descriptorSetBinding.descriptorType = descriptorSetLayoutBinding.descriptorType;

				if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
					descriptorSetBinding.descriptorBuffers = sequence<DescriptorBuffer>(descriptorSetLayoutBinding.descriptorCount);
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

	ref<PhysicalDevice> GL::Device::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> GL::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queue;
	}

	void GL::Device::resetFence(ref<Trident::Fence> fence) const {
		std::static_pointer_cast<GL::Fence>(fence)->reset();
	}

	void GL::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		DEBUG_DESCRIPTOR_GUARD(writeDescriptorSets);

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<GL::DescriptorSet> descriptorSet = std::static_pointer_cast<GL::DescriptorSet>(writeDescriptorSet.descriptorSet);
			GL::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				DEBUG_ASSERT(writeDescriptorSet.descriptorBuffers.count() > 0);

				std::copy(writeDescriptorSet.descriptorBuffers.begin(), writeDescriptorSet.descriptorBuffers.end(), descriptorSetBinding.descriptorBuffers.begin());
			}
			else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				DEBUG_ASSERT(writeDescriptorSet.descriptorImages.count() > 0);

				std::copy(writeDescriptorSet.descriptorImages.begin(), writeDescriptorSet.descriptorImages.end(), descriptorSetBinding.descriptorImages.begin());
			}
			else {
				ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}
		}
	}

	void GL::Device::waitForFence(ref<Trident::Fence> fence) const {
		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			const ref<GL::Fence> glFence = std::static_pointer_cast<GL::Fence>(fence);

			glFence->create();

			const GLenum status = glClientWaitSync(glFence->sync, GL_SYNC_FLUSH_COMMANDS_BIT, max<uint64>());

			DEBUG_ASSERT(status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void GL::Device::waitIdle() const {
		glFinish();
	}
}