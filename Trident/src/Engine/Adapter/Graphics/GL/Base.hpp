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

#if !defined(GL_SOURCE_FROM_SPIRV)
	// TODO: Remove when TSL is ready.
	// Used for custom entrypoints within a GLSL shader.
	#include <regex>
#endif

#include "Engine/Adapter/Graphics/GL/procedures.hpp"

namespace Trident::GL {
	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		typedef HGLRC (*PFNWGLCREATECONTEXTPROC)(HDC);
		typedef BOOL (*PFNWGLDELETECONTEXTPROC)(HGLRC);
		typedef HDC (*PFNWGLGETCURRENTDCPROC)();
		typedef PROC (*PFNWGLGETPROCADDRESSPROC)(LPCSTR);
		typedef BOOL (*PFNWGLMAKECURRENTPROC)(HDC, HGLRC);
	#endif

	class BeginRenderPassCommand;
	class BindComputePipelineCommand;
	class BindDescriptorSetsCommand;
	class BindIndexBufferCommand;
	class BindGraphicsPipelineCommand;
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
	class InsertMemoryBarriersCommand;
	class NextSubpassCommand;
	class SetLineWidthCommand;
	class SetScissorCommand;
	class SetViewportCommand;

	class BaseBuffer;
	class CommandBuffer;
	class CommandEncoder;
	class ComputePipeline;
	class DescriptorSet;
	class Device;
	class Fence;
	class Framebuffer;
	class GraphicsAdapter;
	class GraphicsPipeline;
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class Queue;
	class RenderPass;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Swapchain;

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		template<typename Procedure>
		static Procedure loadWglProcedure(view<char8> procedureName) {
			static const PlatformLibrary library = Platform::openLibrary("opengl32.dll");

			return library.loadProcedure<Procedure>(procedureName);
		}
	#endif

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		template<typename Procedure>
		static Procedure loadProcedure(view<char8> procedureName) {
			static const GL::PFNWGLGETPROCADDRESSPROC wglGetProcAddress = loadWglProcedure<GL::PFNWGLGETPROCADDRESSPROC>("wglGetProcAddress");

			const PROC procedure = wglGetProcAddress(&procedureName[0]);

			// See https://wikis.khronos.org/opengl/Load_OpenGL_Functions.
			if (procedure == NULL || procedure == (void*) -1 || procedure == (void*) 1 || procedure == (void*) 2 || procedure == (void*) 3) {
				return loadWglProcedure<Procedure>(procedureName);
			}

			return function_cast<Procedure>(procedure);
		}
	#endif
}