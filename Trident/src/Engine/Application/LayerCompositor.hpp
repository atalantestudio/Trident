// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/CommandEncoder.hpp"
#include "Engine/API/Rendering/Descriptor/InstanceDescriptor.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/Fence.hpp"
#include "Engine/API/Rendering/Instance.hpp"
#include "Engine/API/Rendering/PhysicalDevice.hpp"
#include "Engine/API/Rendering/Semaphore.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class LayerCompositor {
		private:
			// NOTE
			// Since Vulkan SDK 1.4.313, having less semaphores than swapchain images can cause the warning below to appear:
			// "VkSemaphore is being signaled by VkQueue, but it may still be in use by VkSwapchainKHR".
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

		public:
			explicit LayerCompositor(const Window* window, const GraphicsAdapter* adapter, const ProjectDescriptor& projectDescriptor, const ApplicationProperties& applicationProperties) :
				window(window),
				adapter(adapter),
				verticalSync(applicationProperties.VerticalSync)
			{
				InstanceDescriptor instanceDescriptor{};
				instanceDescriptor.projectName = projectDescriptor.name;
				instanceDescriptor.projectVersion = projectDescriptor.version;

				instance = adapter->createInstance(instanceDescriptor);
				physicalDevice = instance->selectPhysicalDevice(applicationProperties.PreferIntegratedGPU);
				surface = instance->createSurface(physicalDevice, window);
				device = physicalDevice->createDevice();

				const VK::Device* _device = static_cast<const VK::Device*>(device.get());

				const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);

				SwapchainDescriptor swapchainDescriptor{};
				swapchainDescriptor.surface = surface;
				swapchainDescriptor.imageCount = MAX_FRAMES_IN_FLIGHT;
				swapchainDescriptor.extent.width = window->getSize()[0];
				swapchainDescriptor.extent.height = window->getSize()[1];
				swapchainDescriptor.verticalSync = verticalSync;

				swapchain = device->createSwapchain(swapchainDescriptor);

				commandEncoders = sequence<ref<CommandEncoder>>(MAX_FRAMES_IN_FLIGHT);
				acquiredNextImageSemaphores = sequence<ref<Semaphore>>(MAX_FRAMES_IN_FLIGHT);
				finishedExecutionSemaphores = sequence<ref<Semaphore>>(MAX_FRAMES_IN_FLIGHT);
				inFlightFences = sequence<ref<Fence>>(MAX_FRAMES_IN_FLIGHT);

				CommandBufferDescriptor inFlightCommandBufferDescriptor{};
				inFlightCommandBufferDescriptor.usage = CommandBufferUsage::AUTO;

				FenceDescriptor inFlightFenceDescriptor{};
				inFlightFenceDescriptor.signaled = true;

				uint32 frameInFlightIndex = 0;

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> name;
				#endif

				while (frameInFlightIndex < MAX_FRAMES_IN_FLIGHT) {
					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight command buffer []", frameInFlightIndex);

						SET_DEBUG_NAME(inFlightCommandBufferDescriptor, name);
					#endif

					commandEncoders[frameInFlightIndex] = graphicsQueue->createCommandEncoder(inFlightCommandBufferDescriptor);
					acquiredNextImageSemaphores[frameInFlightIndex] = device->createSemaphore();
					finishedExecutionSemaphores[frameInFlightIndex] = device->createSemaphore();

					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("In flight fence []", frameInFlightIndex);

						SET_DEBUG_NAME(inFlightFenceDescriptor, name);
					#endif

					inFlightFences[frameInFlightIndex] = device->createFence(inFlightFenceDescriptor);

					frameInFlightIndex += 1;
				}
			}

			PresentStatus render(uint32 frameInFlightIndex, float64 remainingFrameTime) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const ref<CommandEncoder> commandEncoder = commandEncoders[frameInFlightIndex];
				const ref<Semaphore> acquiredNextImageSemaphore = acquiredNextImageSemaphores[frameInFlightIndex];
				const ref<Semaphore> finishedExecutionSemaphore = finishedExecutionSemaphores[frameInFlightIndex];
				const ref<Fence> inFlightFence = inFlightFences[frameInFlightIndex];

				device->waitForFence(inFlightFence);
				device->resetFence(inFlightFence);

				const AcquireNextImageStatus acquireNextImageStatus = device->acquireNextImage(swapchain, acquiredNextImageSemaphore, swapchainImageIndex);

				ENGINE_ASSERT(acquireNextImageStatus == AcquireNextImageStatus::SUBOPTIMAL || acquireNextImageStatus == AcquireNextImageStatus::SUCCESS, "Failed to acquire the next swapchain image.");

				commandEncoder->reset();
				commandEncoder->begin();

				for (const ref<Layer> layer : layers) {
					layer->preRender();
					layer->render(commandEncoder, remainingFrameTime);
					layer->postRender();
				}

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();
				const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);

				graphicsQueue->submit({commandBuffer}, acquiredNextImageSemaphore, finishedExecutionSemaphore, inFlightFence);

				return graphicsQueue->present({swapchain}, {swapchainImageIndex}, finishedExecutionSemaphore);
			}

			void resize() {
				window->waitMessage();

				device->waitIdle();

				SwapchainDescriptor swapchainDescriptor{};
				swapchainDescriptor.surface = surface;
				swapchainDescriptor.imageCount = MAX_FRAMES_IN_FLIGHT;
				swapchainDescriptor.extent.width = window->getSize()[0];
				swapchainDescriptor.extent.height = window->getSize()[1];
				swapchainDescriptor.verticalSync = verticalSync;

				swapchain->update(swapchainDescriptor);
			}

		public:
			const GraphicsAdapter* adapter;

		private:
			const Window* window;

		public:
			ref<Instance> instance;
			ref<PhysicalDevice> physicalDevice;
			ref<Surface> surface;
			ref<Device> device;
			ref<Swapchain> swapchain;

		private:
			sequence<ref<CommandEncoder>> commandEncoders;
			sequence<ref<Semaphore>> acquiredNextImageSemaphores;
			sequence<ref<Semaphore>> finishedExecutionSemaphores;
			sequence<ref<Fence>> inFlightFences;

		public:
			uint32 swapchainImageIndex = 0;

		private:
			bool verticalSync;

		public:
			std::vector<ref<Layer>> layers;
	};
}