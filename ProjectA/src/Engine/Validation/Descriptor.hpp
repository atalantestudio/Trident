#pragma once

// API descriptor validators for debug mode.
namespace ProjectA {
	struct AttachmentImageDescriptor;
	struct BeginRenderPassDescriptor;
	struct BufferDescriptor;
	struct CommandBufferDescriptor;
	struct ComputePipelineDescriptor;
	struct FramebufferDescriptor;
	struct ImageDescriptor;
	struct ImageViewDescriptor;
	struct PipelineLayoutDescriptor;
	struct RenderPassDescriptor;
	struct RenderPipelineDescriptor;
	struct SamplerDescriptor;
	struct ShaderModuleDescriptor;
	struct Subpass;
	struct SwapchainDescriptor;

	void validateDescriptor(const AttachmentImageDescriptor& descriptor);
	void validateDescriptor(const BeginRenderPassDescriptor& descriptor);
	void validateDescriptor(const BufferDescriptor& descriptor);
	void validateDescriptor(const CommandBufferDescriptor& descriptor);
	void validateDescriptor(const ComputePipelineDescriptor& descriptor);
	void validateDescriptor(const FramebufferDescriptor& descriptor);
	void validateDescriptor(const ImageDescriptor& descriptor);
	void validateDescriptor(const ImageViewDescriptor& descriptor);
	void validateDescriptor(const PipelineLayoutDescriptor& descriptor);
	void validateDescriptor(const RenderPassDescriptor& descriptor);
	void validateDescriptor(const RenderPipelineDescriptor& descriptor);
	void validateDescriptor(const SamplerDescriptor& descriptor);
	void validateDescriptor(const ShaderModuleDescriptor& descriptor);
	void validateDescriptor(const Subpass& subpass);
	void validateDescriptor(const SwapchainDescriptor& descriptor);

	#ifdef BUILD_MODE_DEBUG
		#define DEBUG_VALIDATE_DESCRIPTOR validateDescriptor
	#else
		#define DEBUG_VALIDATE_DESCRIPTOR
	#endif
}