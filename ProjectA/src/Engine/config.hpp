#pragma once

namespace ProjectA {
	#define PROJECT_ROOT_DIRECTORY "/"

	#define GPU_TYPE_DISCRETE 0
	#define GPU_TYPE_INTEGRATED 1

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

	// TODO(review): Replace in favor of custom OpenGL version (see below).
	// If enabled, the OpenGL adapter will use Direct State Access procedures (if available).
	// If disabled, the OpenGL adapter will use standard procedures.
	#define OPENGL_USE_DSA

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define OPENGL_USE_SPIRV

	#define OPENGL_VERSION_1_2 120
	#define OPENGL_VERSION_1_3 130
	#define OPENGL_VERSION_1_4 140
	#define OPENGL_VERSION_1_5 150
	#define OPENGL_VERSION_2_0 200
	#define OPENGL_VERSION_2_1 210
	#define OPENGL_VERSION_3_0 300
	#define OPENGL_VERSION_3_1 310
	#define OPENGL_VERSION_3_2 320
	#define OPENGL_VERSION_3_3 330
	#define OPENGL_VERSION_4_0 400
	#define OPENGL_VERSION_4_1 410
	#define OPENGL_VERSION_4_2 420
	#define OPENGL_VERSION_4_3 430
	#define OPENGL_VERSION_4_4 440
	#define OPENGL_VERSION_4_5 450
	#define OPENGL_VERSION_4_6 460

	// Desired OpenGL version. Note that the current device might not be able to load this version.
	#define OPENGL_VERSION OPENGL_VERSION_4_6

	#define VULKAN_VERSION_1_0 10
	#define VULKAN_VERSION_1_1 11
	#define VULKAN_VERSION_1_2 12
	#define VULKAN_VERSION_1_3 13

	// Desired Vulkan version. Note that the current device might not be able to load this version.
	#define VULKAN_VERSION VULKAN_VERSION_1_0

	#ifdef DEBUG_ENABLE_MESSENGER
		#if OPENGL_VERSION < OPENGL_VERSION_4_3
			#error "Debug messenger cannot be enabled below OpenGL 4.3."
		#endif
	#endif
}