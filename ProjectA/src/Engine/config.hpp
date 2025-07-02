#pragma once

namespace ProjectA {
	#define PROJECT_ROOT_DIRECTORY "/"

	#define GPU_TYPE_DISCRETE 0
	#define GPU_TYPE_INTEGRATED 1

	// The physical device preferred by the application.
	// There is no guarantee that it will be selected at runtime.
	#define PREFERRED_GPU_TYPE GPU_TYPE_DISCRETE

	// TODO: Implement.
	// Verify the validity of the table checksums in a TTF file.
	// #define TTF_PARSER_VERIFY_CHECKSUM

	// Toggle OpenGL graphics adapter.
	#define ENABLE_OPENGL_ADAPTER

	// Toggle Vulkan graphics adapter.
	#define ENABLE_VULKAN_ADAPTER

	// If enabled, glyphs with a single contour are checked to ensure that the contour is clockwise.
	// If not, a warning is outputted and the contour's winding order is inverted.
	#define TTF_FIX_INVERTED_CONTOURS

	// If enabled, the OpenGL adapter will use Direct State Access procedures (if available).
	// If disabled, the OpenGL adapter will use standard procedures.
	#define OPENGL_USE_DSA

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define OPENGL_USE_SPIRV
}