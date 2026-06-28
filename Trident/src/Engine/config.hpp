// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/config_defines.hpp"

namespace Trident {
	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		// Toggle DirectX 12 graphics adapter.
		#define ENABLE_DX12_ADAPTER

		// If enabled, DX12 shaders will be sourced from SPIR-V binaries.
		// If disabled, they will be sourced from HLSL strings.
		#define DX12_SOURCE_FROM_SPIRV

		// Prioritize CPU graphics adapters.
		// #define DX12_PREFER_SOFTWARE_ADAPTER
	#endif

	// Toggle OpenGL graphics adapter.
	#define ENABLE_GL_ADAPTER

	// Whether to prioritize integrated GPUs over discrete GPUs when using the OpenGL graphics adapter.
	// NOTE: Use this instead of the PreferIntegratedGPU project property. GPU selection for a GL context can't be done at runtime.
	// TODO(graphics/gl/igpu): Fix OpenGL adapter never using integrated GPU (works on HP Victus laptop).
	// #define GL_PREFER_INTEGRATED_GPU

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define GL_SOURCE_FROM_SPIRV

	/// (OpenGL) The alignment of a resource inside a memory allocation.
	#define GL_RESOURCE_ALIGNMENT 4

	// Toggle Vulkan graphics adapter.
	#define ENABLE_VK_ADAPTER

	#if defined(BUILD_MODE_DEBUG)
		/// Keeps track of the size of the allocated memory.
		#define DEBUG_COUNT_ALLOCATED_MEMORY_SIZE

		// Toggle debug messages from the graphics API.
		#define DEBUG_ENABLE_MESSENGER

		// Toggle validation layers for graphics APIs that support them.
		// TODO: Do not mix with ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER, which toggles a custom validation layer.
		#define DEBUG_ENABLE_VALIDATION_LAYERS

		/// Toggle to enumerate various Vulkan objects.
		// #define DEBUG_ENUMERATE_DEVICE_EXTENSIONS
		// #define DEBUG_ENUMERATE_MEMORY_HEAPS
		// #define DEBUG_ENUMERATE_MEMORY_TYPES
		// #define DEBUG_ENUMERATE_PHYSICAL_DEVICES
		// #define DEBUG_ENUMERATE_QUEUE_FAMILY_PROPERTIES
		// #define DEBUG_ENUMERATE_VALIDATION_LAYERS
	#endif

	#if defined(BUILD_MODE_DEBUG)
		/// If enabled, all graphics API function calls will be intercepted by the engine's internal validation layer.
		#define ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER
	#endif

	// If enabled, glyphs with a single contour are checked to ensure that the contour is clockwise.
	// If not, a warning is outputted and the contour's winding order is inverted.
	#define TTF_FIX_INVERTED_CONTOURS

	// Desired OpenGL version. Runtime support is not guaranteed.
	#define OPENGL_VERSION OPENGL_VERSION_4_6

	// Desired Vulkan version. Runtime support is not guaranteed.
	#define VULKAN_VERSION VULKAN_VERSION_1_0

	#if defined(BUILD_MODE_DEBUG)
		#define VULKAN_USE_VK_EXT_debug_utils
	#endif

	#define VULKAN_USE_VK_KHR_surface
	#define VULKAN_USE_VK_KHR_swapchain

	#if defined(ENABLE_GL_ADAPTER)
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

	#if defined(BUILD_MODE_DEBUG)
		#if defined(ENABLE_DX12_ADAPTER)
			#define DEBUG_DX12_REPORT_LIVE_OBJECTS
		#endif
	#endif
}