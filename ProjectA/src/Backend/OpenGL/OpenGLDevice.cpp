#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLDescriptorSet.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLFence.hpp"

namespace ProjectA {
	OpenGLDevice::OpenGLDevice(const OpenGLInstance& instance) :
		physicalDevice(createRef<OpenGLPhysicalDevice>()),
		queue(createRef<OpenGLQueue>(instance, *this))
	{}

	AcquireNextImageStatus OpenGLDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		// NOTE
		// WGL handles double buffering at a lower level.
		// The device implementation doesn't need to emulate this behavior and can always return 0.
		imageIndex = 0;

		return AcquireNextImageStatus::SUCCESS;
	}

	sequence<ref<DescriptorSet>> OpenGLDevice::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		sequence<ref<DescriptorSet>> descriptorSets(descriptorSetLayouts.count);

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count; descriptorSetIndex += 1) {
			const ref<DescriptorSetLayout>& descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			ref<OpenGLDescriptorSet>& descriptorSet = createRef<OpenGLDescriptorSet>();
			descriptorSet->bindings = sequence<OpenGLDescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count);

			for (uint32 descriptorSetBindingIndex = 0; descriptorSetBindingIndex < descriptorSet->bindings.count; descriptorSetBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetBindingIndex];

				OpenGLDescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[descriptorSetBindingIndex];
				descriptorSetBinding.descriptorType = descriptorSetLayoutBinding.descriptorType;

				if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
					descriptorSetBinding.descriptorBuffers = sequence<DescriptorBuffer>(descriptorSetLayoutBinding.descriptorCount);
				}
				else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
					descriptorSetBinding.descriptorImages = sequence<DescriptorImage>(descriptorSetLayoutBinding.descriptorCount);
				}
				else {
					ENGINE_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
				}
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;
		}

		return descriptorSets;
	}

	ref<PhysicalDevice> OpenGLDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> OpenGLDevice::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queue;
	}

	void OpenGLDevice::resetFence(const ref<Fence>& fence) const {
		const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

		glFence->reset();
	}

	void OpenGLDevice::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<OpenGLDescriptorSet>& descriptorSetConst = std::static_pointer_cast<OpenGLDescriptorSet>(writeDescriptorSet.descriptorSet);

			// TODO: Find another way without using const_cast.
			ref<OpenGLDescriptorSet>& descriptorSet = const_cast<ref<OpenGLDescriptorSet>&>(descriptorSetConst);

			OpenGLDescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				DEBUG_ASSERT(writeDescriptorSet.descriptorBuffers.count > 0);

				std::copy(writeDescriptorSet.descriptorBuffers.begin(), writeDescriptorSet.descriptorBuffers.end(), descriptorSetBinding.descriptorBuffers.begin());
			}
			else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				DEBUG_ASSERT(writeDescriptorSet.descriptorImages.count > 0);

				std::copy(writeDescriptorSet.descriptorImages.begin(), writeDescriptorSet.descriptorImages.end(), descriptorSetBinding.descriptorImages.begin());
			}
			else {
				ENGINE_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}
		}
	}

	void OpenGLDevice::waitForFence(const ref<Fence>& fence) const {
		#if OPENGL_VERSION >= OPENGL_VERSION_3_2
			const ref<OpenGLFence>& glFence = std::static_pointer_cast<OpenGLFence>(fence);

			glFence->create();

			const GLenum status = glClientWaitSync(glFence->object, GL_SYNC_FLUSH_COMMANDS_BIT, max<uint64>());

			DEBUG_ASSERT(status == GL_CONDITION_SATISFIED || status == GL_ALREADY_SIGNALED);
		#else
			ENGINE_ERROR_UNSUPPORTED();
		#endif
	}

	void OpenGLDevice::waitIdle() const {}
}