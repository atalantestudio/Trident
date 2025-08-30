#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanFramebuffer.hpp"
#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

#include "Backend/Vulkan/MemoryAllocator.hpp"

namespace ProjectA {
	VulkanGraphicsAdapter::VulkanGraphicsAdapter(const ApplicationManifest& applicationManifest, const Window& window) :
		GraphicsAdapter(window)
	{
		instance = createRef<VulkanInstance>(applicationManifest);

		loadProcedures();

		surface = createRef<VulkanSurface>(instance->object, window);
		physicalDevice = createRef<VulkanPhysicalDevice>(instance->selectPhysicalDevice());
		device = createRef<VulkanDevice>(*physicalDevice, *surface);

		surface->setPhysicalDevice(physicalDevice->object);
		surface->checkPresentQueueFamilyIndex(physicalDevice->graphicsQueueFamilyIndex);

		MemoryAllocator::initialize(*physicalDevice);
	}

	VulkanGraphicsAdapter::~VulkanGraphicsAdapter() {
		MemoryAllocator::destroy();
	}

	ref<Buffer> VulkanGraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VulkanBuffer>(device->object, descriptor);
	}

	ref<ComputePipeline> VulkanGraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<VulkanComputePipeline>(device->object, descriptor);
	}

	ref<Fence> VulkanGraphicsAdapter::createFence() const {
		return createRef<VulkanFence>(device->object);
	}

	ref<Framebuffer> VulkanGraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return createRef<VulkanFramebuffer>(device->object, descriptor);
	}

	ref<Image> VulkanGraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<VulkanImage>(device->object, descriptor);
	}

	ref<PipelineLayout> VulkanGraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device->object, descriptor);
	}

	ref<RenderPass> VulkanGraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VulkanRenderPass>(*physicalDevice, *surface, *device, descriptor);
	}

	ref<RenderPipeline> VulkanGraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device->object, descriptor);
	}

	ref<Sampler> VulkanGraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<VulkanSampler>(device->object, descriptor);
	}

	ref<Semaphore> VulkanGraphicsAdapter::createSemaphore() const {
		return createRef<VulkanSemaphore>(device->object);
	}

	ref<ShaderModule> VulkanGraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<VulkanShaderModule>(device->object, code, descriptor);
	}

	ref<Swapchain> VulkanGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VulkanSwapchain>(*physicalDevice, *surface, *device, descriptor);
	}

	ref<Device> VulkanGraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> VulkanGraphicsAdapter::getSurface() const {
		return surface;
	}

	void* VulkanGraphicsAdapter::getProcedure(std::string_view procedureName) const {
		static const PlatformLibrary vulkan1Library = Platform::openLibrary("vulkan-1.dll");
		static const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = pointer_cast<PFN_vkGetInstanceProcAddr>(vulkan1Library.getProcedure("vkGetInstanceProcAddr"));

		const PFN_vkVoidFunction procedure = vkGetInstanceProcAddr(instance->object, procedureName.data());

		ENGINE_ASSERT(procedure != nullptr, "Cannot find address for procedure \"[]\".", procedureName);

		return procedure;
	}

	void VulkanGraphicsAdapter::loadProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(getProcedure(#procedure))

		// TODO(review)
		LOAD_PROCEDURE(vkCmdDraw);

		#undef LOAD_PROCEDURE
	}
}