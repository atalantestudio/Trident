// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include <vulkan/vulkan_win32.h>
#else
	#error "Unsupported operating system.'
#endif

#if defined(BUILD_MODE_DEBUG)
	#include <vulkan/vk_enum_string_helper.h>
#endif

#include "Engine/Adapter/Graphics/VK/procedures.hpp"

namespace Trident::VK {
	struct MemoryAllocation;
	struct MemorySuballocation;

	class Buffer;
	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class ComputePipeline;
	class DescriptorSet;
	class DescriptorSetLayout;
	class Device;
	class Fence;
	class Framebuffer;
	class GraphicsAdapter;
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class Pipeline;
	class PipelineLayout;
	class Queue;
	class RenderPass;
	class RenderPipeline;
	class Sampler;
	class Semaphore;
	class ShaderModule;
	class Surface;
	class Swapchain;
}