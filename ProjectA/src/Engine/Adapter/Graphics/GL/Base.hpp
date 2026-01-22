// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <GL/glcorearb.h>

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include <gl/GL.h>
#endif

#include <GL/glext.h>

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include <GL/wglext.h>
#endif

#ifndef GL_SOURCE_FROM_SPIRV
	// TODO: Remove when TSL is ready.
	// NOTE: <regex> is used for custom entrypoints within a GLSL shader.
	#include <regex>
#endif

#include "Engine/Adapter/Graphics/GL/procedures.hpp"

namespace ProjectA::GL {
	class BeginRenderPassCommand;
	class BindComputePipelineCommand;
	class BindDescriptorSetsCommand;
	class BindIndexBufferCommand;
	class BindRenderPipelineCommand;
	class BindVertexBufferCommand;
	class Command;
	class CopyBufferToBufferCommand;
	class CopyBufferToImageCommand;
	class CopyImageToBufferCommand;
	class DispatchCommand;
	class DispatchIndirectCommand;
	class DrawCommand;
	class DrawIndexedCommand;
	class DrawIndexedIndirectCommand;
	class DrawIndirectCommand;
	class EndRenderPassCommand;
	class NextSubpassCommand;
	class SetImageMemoryBarrierCommand;
	class SetLineWidthCommand;
	class SetScissorCommand;
	class SetViewportCommand;

	class Buffer;
	class CommandBuffer;
	class CommandEncoder;
	class ComputePipeline;
	class DescriptorSet;
	class Device;
	class Fence;
	class Framebuffer;
	class GraphicsAdapter;
	class Image;
	class ImageView;
	class Instance;
	class PhysicalDevice;
	class Queue;
	class RenderPass;
	class RenderPipeline;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Swapchain;
}