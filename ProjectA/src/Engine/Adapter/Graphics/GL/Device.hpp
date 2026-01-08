#pragma once

#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/GL/Queue.hpp"

#include "Engine/API/Rendering/Device.hpp"

namespace ProjectA {
	class OpenGLDevice final : public Device {
		public:
			explicit OpenGLDevice(const OpenGLInstance& instance);

			AcquireNextImageStatus acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const final override;

			sequence<ref<DescriptorSet>> allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) final override;

			ref<PhysicalDevice> getPhysicalDevice() const final override;

			ref<Queue> getQueue(QueueFamilyCapability queueFamilyCapability) const final override;

			void resetFence(ref<Fence> fence) const final override;

			void updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const final override;

			void waitForFence(ref<Fence> fence) const final override;

			void waitIdle() const final override;

		private:
			ref<OpenGLPhysicalDevice> physicalDevice;
			ref<OpenGLQueue> queue;
	};
}