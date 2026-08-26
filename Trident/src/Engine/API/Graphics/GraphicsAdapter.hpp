// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"
#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/BufferView.hpp"
#include "Engine/API/Graphics/CommandBuffer.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"
#include "Engine/API/Graphics/CommandPool.hpp"
#include "Engine/API/Graphics/ComputePipeline.hpp"
#include "Engine/API/Graphics/Descriptor/InstanceDescriptor.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/DispatchIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndexedIndirectCommand.hpp"
#include "Engine/API/Graphics/DrawIndirectCommand.hpp"
#include "Engine/API/Graphics/Fence.hpp"
#include "Engine/API/Graphics/Framebuffer.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageView.hpp"
#include "Engine/API/Graphics/Instance.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/PipelineLayout.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/API/Graphics/RenderPass.hpp"
#include "Engine/API/Graphics/Sampler.hpp"
#include "Engine/API/Graphics/Semaphore.hpp"
#include "Engine/API/Graphics/ShaderModule.hpp"
#include "Engine/API/Graphics/Status.hpp"
#include "Engine/API/Graphics/Swapchain.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident {
	class GraphicsAdapter {
		public:
			static ConsoleLogger logger;

		public:
			explicit GraphicsAdapter(const ApplicationProperties& properties);

			GraphicsAPI getApi() const;

			ref<Instance> createInstance(const InstanceDescriptor& descriptor) const;

		public:
			AcquireNextImageImplementation acquireNextImageImplementation;
			AllocateDescriptorSetsImplementation allocateDescriptorSetsImplementation;
			AllocateMemoryImplementation allocateMemoryImplementation;
			BeginImplementation beginImplementation;
			BeginRenderPassImplementation beginRenderPassImplementation;
			BindComputePipelineImplementation bindComputePipelineImplementation;
			BindDescriptorSetsImplementation bindDescriptorSetsImplementation;
			BindGraphicsPipelineImplementation bindGraphicsPipelineImplementation;
			BindIndexBufferImplementation bindIndexBufferImplementation;
			BindVertexBufferImplementation bindVertexBufferImplementation;
			CopyBufferToBufferImplementation copyBufferToBufferImplementation;
			CopyBufferToImageImplementation copyBufferToImageImplementation;
			CopyImageToBufferImplementation copyImageToBufferImplementation;
			CreateBufferImplementation createBufferImplementation;
			CreateCommandEncoderImplementation createCommandEncoderImplementation;
			CreateCommandPoolImplementation createCommandPoolImplementation;
			CreateComputePipelineImplementation createComputePipelineImplementation;
			CreateDescriptorSetLayoutImplementation createDescriptorSetLayoutImplementation;
			CreateDeviceImplementation createDeviceImplementation;
			CreateFenceImplementation createFenceImplementation;
			CreateFramebufferImplementation createFramebufferImplementation;
			CreateGraphicsPipelineImplementation createGraphicsPipelineImplementation;
			CreateImageImplementation createImageImplementation;
			CreateImageViewImplementation createImageViewImplementation;
			CreateInstanceImplementation createInstanceImplementation;
			CreateMemoryAllocatorImplementation createMemoryAllocatorImplementation;
			CreatePipelineLayoutImplementation createPipelineLayoutImplementation;
			CreateRenderPassImplementation createRenderPassImplementation;
			CreateSamplerImplementation createSamplerImplementation;
			CreateSemaphoreImplementation createSemaphoreImplementation;
			CreateShaderModuleFromAssetImplementation createShaderModuleFromAssetImplementation;
			CreateShaderModuleFromCodeImplementation createShaderModuleFromCodeImplementation;
			CreateSurfaceImplementation createSurfaceImplementation;
			CreateSwapchainImplementation createSwapchainImplementation;
			DestroyBufferImplementation destroyBufferImplementation;
			DestroyCommandEncoderImplementation destroyCommandEncoderImplementation;
			DestroyCommandPoolImplementation destroyCommandPoolImplementation;
			DestroyComputePipelineImplementation destroyComputePipelineImplementation;
			DestroyDescriptorSetLayoutImplementation destroyDescriptorSetLayoutImplementation;
			DestroyDeviceImplementation destroyDeviceImplementation;
			DestroyFenceImplementation destroyFenceImplementation;
			DestroyFramebufferImplementation destroyFramebufferImplementation;
			DestroyGraphicsPipelineImplementation destroyGraphicsPipelineImplementation;
			DestroyImageImplementation destroyImageImplementation;
			DestroyImageViewImplementation destroyImageViewImplementation;
			DestroyInstanceImplementation destroyInstanceImplementation;
			DestroyPipelineLayoutImplementation destroyPipelineLayoutImplementation;
			DestroyQueueImplementation destroyQueueImplementation;
			DestroyRenderPassImplementation destroyRenderPassImplementation;
			DestroySamplerImplementation destroySamplerImplementation;
			DestroySemaphoreImplementation destroySemaphoreImplementation;
			DestroyShaderModuleImplementation destroyShaderModuleImplementation;
			DestroySurfaceImplementation destroySurfaceImplementation;
			DestroySwapchainImplementation destroySwapchainImplementation;
			DeviceWaitIdleImplementation deviceWaitIdleImplementation;
			DispatchImplementation dispatchImplementation;
			DispatchIndirectImplementation dispatchIndirectImplementation;
			DrawImplementation drawImplementation;
			DrawIndirectImplementation drawIndirectImplementation;
			DrawIndexedImplementation drawIndexedImplementation;
			DrawIndexedIndirectImplementation drawIndexedIndirectImplementation;
			EndImplementation endImplementation;
			EndRenderPassImplementation endRenderPassImplementation;
			FindMemoryTypeIndexImplementation findMemoryTypeIndexImplementation;
			GetBufferMemoryRequirementsImplementation getBufferMemoryRequirementsImplementation;
			GetImageMemoryRequirementsImplementation getImageMemoryRequirementsImplementation;
			GetPhysicalDeviceMemoryTypeCountImplementation getPhysicalDeviceMemoryTypeCountImplementation;
			GetPhysicalDeviceNameImplementation getPhysicalDeviceNameImplementation;
			InsertMemoryBarriersImplementation insertMemoryBarriersImplementation;
			MapBufferImplementation mapBufferImplementation;
			NextSubpassImplementation nextSubpassImplementation;
			PresentImplementation presentImplementation;
			QueueWaitIdleImplementation queueWaitIdleImplementation;
			ResetImplementation resetImplementation;
			ResetFenceImplementation resetFenceImplementation;
			SelectPhysicalDeviceImplementation selectPhysicalDeviceImplementation;
			SetLineWidthImplementation setLineWidthImplementation;
			SetScissorImplementation setScissorImplementation;
			SetViewportImplementation setViewportImplementation;
			SuballocateBufferMemoryImplementation suballocateBufferMemoryImplementation;
			SuballocateImageMemoryImplementation suballocateImageMemoryImplementation;
			SubmitImplementation submitImplementation;
			Submit2Implementation submit2Implementation;
			UnmapBufferImplementation unmapBufferImplementation;
			UpdateDescriptorSetsImplementation updateDescriptorSetsImplementation;
			UpdateSwapchainImplementation updateSwapchainImplementation;
			WaitForFenceImplementation waitForFenceImplementation;

			TestCreateBLASTriangleGeometryImplementation testCreateBLASTriangleGeometryImplementation;

		private:
			GraphicsAPI api;
	};
}