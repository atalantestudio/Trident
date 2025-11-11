#pragma once

#include "Engine/config_defines.hpp"

namespace ProjectA {
	// The physical device preferred by the application.
	// There is no guarantee that it will be selected at runtime.
	#define PREFERRED_GPU_TYPE GPU_TYPE_DISCRETE

	// Toggle OpenGL graphics adapter.
	#define ENABLE_OPENGL_ADAPTER

	// Toggle Vulkan graphics adapter.
	#define ENABLE_VULKAN_ADAPTER

	// Toggle debug messages from the graphics API.
	#define DEBUG_ENABLE_MESSENGER

	// Toggle validation layers for graphics APIs that support them.
	#define DEBUG_ENABLE_VALIDATION_LAYERS

	// If enabled, glyphs with a single contour are checked to ensure that the contour is clockwise.
	// If not, a warning is outputted and the contour's winding order is inverted.
	#define TTF_FIX_INVERTED_CONTOURS

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define OPENGL_USE_SPIRV

	// If enabled, OpenGL objects are cleanly unbound from their target after being used.
	#define OPENGL_UNBIND_AFTER_USE

	// Desired OpenGL version. Note that the runtime device might not have this version.
	#define OPENGL_VERSION OPENGL_VERSION_4_6

	// Desired Vulkan version. Note that the runtime device might not have this version.
	#define VULKAN_VERSION VULKAN_VERSION_1_0

	#define VULKAN_USE_VK_KHR_surface
	#define VULKAN_USE_VK_KHR_swapchain

	#ifdef ENABLE_OPENGL_ADAPTER
		#if OPENGL_VERSION < OPENGL_VERSION_2_0
			#error "OpenGL versions older than 2.0 cannot be used."
		#endif

		#if defined(DEBUG_ENABLE_MESSENGER) && OPENGL_VERSION < OPENGL_VERSION_4_3
			#error "Debug messenger cannot be enabled below OpenGL 4.3."
		#endif

		#if defined(OPENGL_USE_SPIRV) && OPENGL_VERSION < OPENGL_VERSION_4_6
			#error "SPIR-V shaders cannot be used below OpenGL 4.6."
		#endif
	#endif
}