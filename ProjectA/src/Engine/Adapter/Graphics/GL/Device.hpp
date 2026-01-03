#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"

#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class OpenGLDevice final : public Device {
		public:
			explicit OpenGLDevice(const OpenGLInstance& instance);

			AcquireNextImageStatus acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const final override;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(const ref<Fence>& fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(const ref<Fence>& fence) const final override;

			void waitIdle() const final override;

		private:
			const ref<OpenGLPhysicalDevice> physicalDevice;
			const ref<OpenGLQueue> queue;
	};
}