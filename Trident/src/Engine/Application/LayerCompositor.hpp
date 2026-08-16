// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/Descriptor/InstanceDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/API/Graphics/Semaphore.hpp"
#include "Engine/API/Graphics/Surface.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"
#include "Engine/API/Window/LayerGroup/RenderPassLayerGroup.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
#include "Engine/Application/ProjectDescriptor.hpp"
#include "Engine/Layer/Layer.hpp"

namespace Trident {
	class LayerCompositor {
		private:
			// NOTE
			// Since Vulkan SDK 1.4.313, having less semaphores than swapchain images will trigger a validation warning.
			// See https://vulkan.lunarg.com/doc/view/1.4.321.1/windows/antora/spec/latest/chapters/cmdbuffers.html#VUID-vkQueueSubmit-pSignalSemaphores-00067.
			static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

		public:
			LayerCompositor() = default;

			explicit LayerCompositor(const Window* window, ref<GraphicsAdapter> adapter, const ProjectDescriptor& projectDescriptor, const ApplicationProperties& applicationProperties) :
				adapter(adapter),
				window(window),
				verticalSync(applicationProperties.VerticalSync),
				layers(MAX_LAYER_COUNT),
				layerGroups(MAX_LAYER_GROUP_COUNT)
			{
				instance = adapter->createInstance({
					.projectName = projectDescriptor.name,
					.projectVersion = projectDescriptor.version,
				});
				physicalDevice = instance->selectPhysicalDevice(applicationProperties.PreferIntegratedGPU);
				surface = instance->createSurface(physicalDevice, window);

				// OpenGL requires setting the window before loading the procedures.
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

				#if defined(DEBUG_OBJECT_NAME)
					sequence<char8> inFlightName;
				#endif

				uint32 frameInFlightIndex = 0;

				while (frameInFlightIndex < MAX_FRAMES_IN_FLIGHT) {
					#if defined(DEBUG_OBJECT_NAME)
						inFlightName = Logger::format("In flight []", frameInFlightIndex);

						inFlightCommandBufferDescriptor.name = inFlightName;
						inFlightFenceDescriptor.name = inFlightName;
					#endif

					inFlightCommandEncoders[frameInFlightIndex] = graphicsQueue->createCommandEncoder(inFlightCommandBufferDescriptor);
					acquiredNextImageSemaphores[frameInFlightIndex] = device->createSemaphore();
					finishedExecutionSemaphores[frameInFlightIndex] = device->createSemaphore();
					inFlightFences[frameInFlightIndex] = device->createFence(inFlightFenceDescriptor);

					frameInFlightIndex += 1;
				}
			}

			view<ref<Layer>> getLayers() const {
				return view<ref<Layer>>(&layers[0], layerCount);
			}

			view<ref<LayerGroup>> getLayerGroups() const {
				return view<ref<LayerGroup>>(&layerGroups[0], layerGroupCount);
			}

			void addLayer(ref<Layer>&& layer) {
				layers[layerCount] = layer;
				layerCount += 1;
			}

			void addLayerGroup(ref<LayerGroup> layerGroup) {
				layerGroups[layerGroupCount] = layerGroup;
				layerGroupCount += 1;
			}

			void initializeLayerGroups() const {
				for (ref<LayerGroup> layerGroup : getLayerGroups()) {
					layerGroup->initialize();
				}
			}

			void initializeLayers() const {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const view<ref<Layer>> layerView = getLayers();
				const ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();
				const uint32 memoryTypeCount = physicalDevice->getMemoryTypeCount();

				sequence<uint64> allocationSizesPerMemoryType(memoryTypeCount);
				sequence<uint64> allocationAlignmentsPerMemoryType(memoryTypeCount);
				sequence<uint32> allocationIndicesPerMemoryType(memoryTypeCount);
				uint64 staticBufferCount = 0;
				uint64 staticImageCount = 0;

				for (ref<Layer> layer : layerView) {
					layer->initialize();

					const StaticMemoryDescriptor staticMemoryDescriptor = layer->getStaticMemoryDescriptor();

					staticBufferCount += staticMemoryDescriptor.buffers.count();
					staticImageCount += staticMemoryDescriptor.images.count();
				}

				sequence<ref<Buffer>> staticBuffers(staticBufferCount);
				sequence<ref<Image>> staticImages(staticImageCount);

				staticBufferCount = 0;
				staticImageCount = 0;

				for (ref<Layer> layer : layerView) {
					const StaticMemoryDescriptor staticMemoryDescriptor = layer->getStaticMemoryDescriptor();
					const view<ref<Buffer>> buffers(&staticBuffers[staticBufferCount], staticMemoryDescriptor.buffers.count());
					const view<ref<Image>> images(&staticImages[staticImageCount], staticMemoryDescriptor.images.count());

					for (ref<Buffer> buffer : staticMemoryDescriptor.buffers) {
						staticBuffers[staticBufferCount] = buffer;
						staticBufferCount += 1;
					}

					for (ref<Image> image : staticMemoryDescriptor.images) {
						staticImages[staticImageCount] = image;
						staticImageCount += 1;
					}

					for (ref<Buffer> buffer : buffers) {
						const MemorySuballocation& memorySuballocation = buffer->getMemorySuballocation();

						DEBUG_ASSERT(memorySuballocation.size == 0);

						const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(memorySuballocation.memoryProperties, buffer->getUsage() & BufferUsage::TRANSFER_DST);
						const MemoryRequirements memoryRequirements = memoryAllocator->getMemoryRequirements(buffer);

						uint64& alignment = allocationAlignmentsPerMemoryType[memoryTypeIndex];
						alignment = max(alignment, memoryRequirements.alignment);

						allocationSizesPerMemoryType[memoryTypeIndex] = max(allocationSizesPerMemoryType[memoryTypeIndex], alignment);
						allocationSizesPerMemoryType[memoryTypeIndex] += ceil(memoryRequirements.size, alignment);
					}

					for (ref<Image> image : images) {
						const MemorySuballocation& memorySuballocation = image->getMemorySuballocation();

						DEBUG_ASSERT(memorySuballocation.size == 0);

						const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(memorySuballocation.memoryProperties, image->getUsage() & ImageUsage::TRANSFER_DST);
						const MemoryRequirements memoryRequirements = memoryAllocator->getMemoryRequirements(image);

						uint64& alignment = allocationAlignmentsPerMemoryType[memoryTypeIndex];
						alignment = max(alignment, memoryRequirements.alignment);

						allocationSizesPerMemoryType[memoryTypeIndex] = max(allocationSizesPerMemoryType[memoryTypeIndex], alignment);
						allocationSizesPerMemoryType[memoryTypeIndex] += ceil(memoryRequirements.size, alignment);
					}
				}

				uint32 memoryTypeIndex = 0;

				while (memoryTypeIndex < memoryTypeCount) {
					const uint64 allocationSize = allocationSizesPerMemoryType[memoryTypeIndex];

					if (allocationSize > 0) {
						const uint64 allocationAlignment = allocationAlignmentsPerMemoryType[memoryTypeIndex];

						allocationIndicesPerMemoryType[memoryTypeIndex] = memoryAllocator->allocate(false, memoryTypeIndex, allocationSize, allocationAlignment);
					}

					memoryTypeIndex += 1;
				}

				for (ref<Buffer> buffer : staticBuffers) {
					const MemorySuballocation& memorySuballocation = buffer->getMemorySuballocation();
					const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(memorySuballocation.memoryProperties, buffer->getUsage() & BufferUsage::TRANSFER_DST);
					const uint32 allocationIndex = allocationIndicesPerMemoryType[memoryTypeIndex];

					memoryAllocator->suballocateStatic(allocationIndex, buffer);
				}

				for (ref<Image> image : staticImages) {
					const MemorySuballocation& memorySuballocation = image->getMemorySuballocation();
					const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(memorySuballocation.memoryProperties, image->getUsage() & ImageUsage::TRANSFER_DST);
					const uint32 allocationIndex = allocationIndicesPerMemoryType[memoryTypeIndex];

					memoryAllocator->suballocateStatic(allocationIndex, image);
				}

				for (ref<Layer> layer : layerView) {
					layer->encodeOneTimeCommands();
				}
			}

			PresentStatus render(uint32 frameInFlightIndex, float64 remainingFrameTime) {
				PROFILE_SCOPE(FUNCTION_SIGNATURE);

				const view<ref<LayerGroup>> layerGroupView = getLayerGroups();
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

				for (ref<LayerGroup> layerGroup : layerGroupView) {
					layerGroup->render(commandEncoder, remainingFrameTime);
				}

				const ref<CommandBuffer> commandBuffer = commandEncoder->end();

				graphicsQueue->submit(commandBuffer, acquiredNextImageSemaphore, finishedExecutionSemaphore, inFlightFence);

				return graphicsQueue->present(swapchain, swapchainImageIndex, finishedExecutionSemaphore);
			}

			void propagateEventFrontToBack(Event& event) {
				uint32 layerIndex = 0;

				while (layerIndex < layerCount) {
					if (event.handled) {
						break;
					}

					layers[layerIndex]->onEvent(event);

					layerIndex += 1;
				}
			}

			void propagateEventBackToFront(Event& event) {
				sint32 layerIndex = static_cast<sint32>(layerCount) - 1;

				while (layerIndex >= 0) {
					layers[layerIndex]->onEvent(event);

					layerIndex -= 1;
				}
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

				const view<ref<LayerGroup>> layerGroupView = getLayerGroups();

				for (ref<LayerGroup> layerGroup : layerGroupView) {
					layerGroup->resize();
				}
			}

		public:
			ref<GraphicsAdapter> adapter;

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

		private:
			sequence<ref<Layer>> layers;
			sequence<ref<LayerGroup>> layerGroups;

		public:
			uint64 layerCount = 0;
			uint64 layerGroupCount = 0;
	};
}