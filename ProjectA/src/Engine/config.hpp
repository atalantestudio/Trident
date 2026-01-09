#pragma once

#include "Engine/config_defines.hpp"

namespace ProjectA {
	// TODO: Implement setting.
	// Prioritize selection of CPU graphics adapters.
	// #define PREFER_SOFTWARE_ADAPTER

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		// Toggle DirectX 12 graphics adapter.
		#define ENABLE_DX12_ADAPTER

		// If enabled, DX12 shaders will be sourced from SPIR-V binaries.
		// If disabled, they will be sourced from HLSL strings.
		#define DX12_SOURCE_FROM_SPIRV
	#endif

	// Toggle OpenGL graphics adapter.
	#define ENABLE_GL_ADAPTER

	// Whether to prioritize integrated GPUs over discrete GPUs.
	// This does not guarantee that an integrated GPU will be selected at runtime.
	// 
	// NOTE: Use this instead of the PreferIntegratedGPU project property. GPU selection for a GL context can only be done at compile-time.
	// #define GL_PREFER_INTEGRATED_GPU

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define GL_SOURCE_FROM_SPIRV

	// Toggle Vulkan graphics adapter.
	#define ENABLE_VK_ADAPTER

	// Toggle debug messages from the graphics API.
	#define DEBUG_ENABLE_MESSENGER

	// Toggle validation layers for graphics APIs that support them.
	#define DEBUG_ENABLE_VALIDATION_LAYERS

	// If enabled, glyphs with a single contour are checked to ensure that the contour is clockwise.
	// If not, a warning is outputted and the contour's winding order is inverted.
	#define TTF_FIX_INVERTED_CONTOURS

	// If enabled, OpenGL objects are cleanly unbound from their target after being used.
	#define OPENGL_UNBIND_AFTER_USE

	// Desired OpenGL version. Note that the runtime device might not have this version.
	#define OPENGL_VERSION OPENGL_VERSION_4_6

	// Desired Vulkan version. Note that the runtime device might not have this version.
	#define VULKAN_VERSION VULKAN_VERSION_1_0

	#define VULKAN_USE_VK_KHR_surface
	#define VULKAN_USE_VK_KHR_swapchain

	#ifdef ENABLE_GL_ADAPTER
		#if OPENGL_VERSION < OPENGL_VERSION_2_0
			#error "OpenGL versions older than 2.0 cannot be used."
		#endif

		#if OPENGL_VERSION < OPENGL_VERSION_4_3 && defined(DEBUG_ENABLE_MESSENGER)
			#error "Debug messenger cannot be enabled below OpenGL 4.3."
		#endif

		#if OPENGL_VERSION < OPENGL_VERSION_4_6 && defined(GL_SOURCE_FROM_SPIRV)
			#error "SPIR-V shaders cannot be used below OpenGL 4.6."
		#endif
	#endif
}