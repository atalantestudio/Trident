// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	#if defined(BUILD_MODE_DEBUG)
		#define SET_DEBUG_NAME(descriptor, value) descriptor.name = value

		class Image;

		struct AttachmentImageDescriptor;
		struct BeginRenderPassDescriptor;
		struct BufferDescriptor;
		struct CommandBufferDescriptor;
		struct ComputePipelineDescriptor;
		struct FramebufferDescriptor;
		struct FenceDescriptor;
		struct ImageDescriptor;
		struct ImageViewDescriptor;
		struct PipelineLayoutDescriptor;
		struct RenderPassDescriptor;
		struct RenderPipelineDescriptor;
		struct SamplerDescriptor;
		struct ShaderModuleDescriptor;
		struct SubpassDescriptor;
		struct SwapchainDescriptor;
		struct WriteDescriptorSet;

		// API descriptor validators for debug mode.
		void DescriptorGuard(const AttachmentImageDescriptor& descriptor);
		void DescriptorGuard(const BeginRenderPassDescriptor& descriptor);
		void DescriptorGuard(const BufferDescriptor& descriptor);
		void DescriptorGuard(const CommandBufferDescriptor& descriptor);
		void DescriptorGuard(const ComputePipelineDescriptor& descriptor);
		void DescriptorGuard(const FenceDescriptor& descriptor);
		void DescriptorGuard(const FramebufferDescriptor& descriptor);
		void DescriptorGuard(const ImageDescriptor& descriptor);
		void DescriptorGuard(const ImageViewDescriptor& descriptor, Image* image);
		void DescriptorGuard(const PipelineLayoutDescriptor& descriptor);
		void DescriptorGuard(const RenderPassDescriptor& descriptor);
		void DescriptorGuard(const RenderPipelineDescriptor& descriptor);
		void DescriptorGuard(const SamplerDescriptor& descriptor);
		void DescriptorGuard(const ShaderModuleDescriptor& descriptor);
		void DescriptorGuard(const SubpassDescriptor& descriptor);
		void DescriptorGuard(const SwapchainDescriptor& descriptor);
		void DescriptorGuard(view<WriteDescriptorSet> writeDescriptorSets);

		#define DEBUG_DESCRIPTOR_GUARD DescriptorGuard
	#else
		#define SET_DEBUG_NAME(descriptor, value)

		#define DEBUG_DESCRIPTOR_GUARD
	#endif
}