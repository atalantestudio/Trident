// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Descriptor/InstanceDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/API/Graphics/Semaphore.hpp"
#include "Engine/API/Graphics/Surface.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class LayerCompositor {
		private:
			// NOTE
			// Since Vulkan SDK 1.4.313, having less semaphores than swapchain images will trigger a validation warning.
			// See https://vulkan.lunarg.com/doc/view/1.4.321.1/windows/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067.
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

		public:
			explicit LayerCompositor(const Window* window, const GraphicsAdapter* adapter, const ProjectDescriptor& projectDescriptor, const ApplicationProperties& applicationProperties) :
				window(window),
				adapter(adapter),
				verticalSync(applicationProperties.VerticalSync)
			{
				instance = adapter->createInstance({
					.projectName = projectDescriptor.name,
					.projectVersion = projectDescriptor.version,
				});
				physicalDevice = instance->selectPhysicalDevice(applicationProperties.PreferIntegratedGPU);
				surface = instance->createSurface(physicalDevice, window);

				ENGINE_INFO("Using physical device \"[]\".", physicalDevice->getName());

				device = physicalDevice->createDevice();
				graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
				swapchain = device->createSwapchain({
					.surface = surface,
					.imageCount = MAX_FRAMES_IN_FLIGHT,
					.extent = window->getExtent(),
					.verticalSync = verticalSync,
				});

				inFlightCommandEncoders = sequence<ref<CommandEncoder>>(MAX_FRAMES_IN_FLIGHT);
				acquiredNextImageSemaphores = sequence<ref<Semaphore>>(MAX_FRAMES_IN_FLIGHT);
				finishedExecutionSemaphores = sequence<ref<Semaphore>>(MAX_FRAMES_IN_FLIGHT);
				inFlightFences = sequence<ref<Fence>>(MAX_FRAMES_IN_FLIGHT);

				CommandBufferDescriptor inFlightCommandBufferDescriptor = {
					.usage = CommandBufferUsage::AUTO,
				};
				FenceDescriptor inFlightFenceDescriptor = {
					.signaled = true,
				};

				#if defined(BUILD_MODE_DEBUG)
					sequence<char8> inFlightCommandEncoderName;
					sequence<char8> inFlightFenceName;
				#endif

				uint32 frameInFlightIndex = 0;

				while (frameInFlightIndex < MAX_FRAMES_IN_FLIGHT) {
					#if defined(BUILD_MODE_DEBUG)
						inFlightCommandEncoderName = Logger::format("In flight command encoder []", frameInFlightIndex);
						inFlightFenceName = Logger::format("In flight fence []", frameInFlightIndex);

						inFlightCommandBufferDescriptor.name = inFlightCommandEncoderName;
						inFlightFenceDescriptor.name = inFlightFenceName;
					#endif

					inFlightCommandEncoders[frameInFlightIndex] = graphicsQueue->createCommandEncoder(inFlightCommandBufferDescriptor);
					acquiredNextImageSemaphores[frameInFlightIndex] = device->createSemaphore();
					finishedExecutionSemaphores[frameInFlightIndex] = device->createSemaphore();
					inFlightFences[frameInFlightIndex] = device->createFence(inFlightFenceDescriptor);

					frameInFlightIndex += 1;
				}
			}

			PresentStatus render(uint32 frameInFlightIndex, float64 remainingFrameTime) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const ref<CommandEncoder> commandEncoder = inFlightCommandEncoders[frameInFlightIndex];
				const ref<Semaphore> acquiredNextImageSemaphore = acquiredNextImageSemaphores[frameInFlightIndex];
				const ref<Semaphore> finishedExecutionSemaphore = finishedExecutionSemaphores[frameInFlightIndex];
				const ref<Fence> inFlightFence = inFlightFences[frameInFlightIndex];

				device->waitForFence(inFlightFence);
				device->resetFence(inFlightFence);

				const AcquireNextImageStatus acquireNextImageStatus = device->acquireNextImage(swapchain, acquiredNextImageSemaphore, swapchainImageIndex);

				// TODO(graphics/acquire-next-image-status): Resize the swapchain if the status is SUBOPTIMAL or ERROR_OUT_OF_DATE.
				ENGINE_ASSERT(acquireNextImageStatus == AcquireNextImageStatus::SUBOPTIMAL || acquireNextImageStatus == AcquireNextImageStatus::SUCCESS, "Failed to acquire the next swapchain image.");

				commandEncoder->reset();
				commandEncoder->begin();

				for (const ref<Layer> layer : layers) {
					layer->preRender();
					layer->render(commandEncoder, remainingFrameTime);
					layer->postRender();
				}

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit({commandBuffer}, acquiredNextImageSemaphore, finishedExecutionSemaphore, inFlightFence);

				return graphicsQueue->present({swapchain}, {swapchainImageIndex}, finishedExecutionSemaphore);
			}

			void resize() {
				window->waitMessage();

				device->waitIdle();

				swapchain->update({
					.surface = surface,
					.imageCount = MAX_FRAMES_IN_FLIGHT,
					.extent = window->getExtent(),
					.verticalSync = verticalSync,
				});
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
			ref<Queue> graphicsQueue;
			ref<Swapchain> swapchain;

		private:
			sequence<ref<CommandEncoder>> inFlightCommandEncoders;
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