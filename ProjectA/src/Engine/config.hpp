#pragma once

namespace ProjectA {
	#define PROJECT_ROOT_DIRECTORY "/"

	#define GPU_TYPE_DISCRETE 0
	#define GPU_TYPE_INTEGRATED 1

	// The physical device preferred by the application.
	// There is no guarantee that it will be selected at runtime.
	#define PREFERRED_GPU_TYPE GPU_TYPE_DISCRETE

	// TODO(review): Validate TTF table checksums.
	// Verify the validity of the table checksums in a TTF file.
	// #define TTF_PARSER_VERIFY_CHECKSUM

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

	/* #ifdef GL_VERSION_1_2
		#define OPENGL_VERSION OPENGL_VERSION_1_2
	#endif

	#ifdef GL_VERSION_1_3
		#define OPENGL_VERSION OPENGL_VERSION_1_3
	#endif

	#ifdef GL_VERSION_1_4
		#define OPENGL_VERSION OPENGL_VERSION_1_4
	#endif

	#ifdef GL_VERSION_1_5
		#define OPENGL_VERSION OPENGL_VERSION_1_5
	#endif

	#ifdef GL_VERSION_2_0
		#define OPENGL_VERSION OPENGL_VERSION_2_0
	#endif

	#ifdef GL_VERSION_2_1
		#define OPENGL_VERSION OPENGL_VERSION_2_1
	#endif

	#ifdef GL_VERSION_3_0
		#define OPENGL_VERSION OPENGL_VERSION_3_0
	#endif

	#ifdef GL_VERSION_3_1
		#define OPENGL_VERSION OPENGL_VERSION_3_1
	#endif

	#ifdef GL_VERSION_3_2
		#define OPENGL_VERSION OPENGL_VERSION_3_2
	#endif

	#ifdef GL_VERSION_3_3
		#define OPENGL_VERSION OPENGL_VERSION_3_3
	#endif

	#ifdef GL_VERSION_4_0
		#define OPENGL_VERSION OPENGL_VERSION_4_0
	#endif

	#ifdef GL_VERSION_4_1
		#define OPENGL_VERSION OPENGL_VERSION_4_1
	#endif

	#ifdef GL_VERSION_4_2
		#define OPENGL_VERSION OPENGL_VERSION_4_2
	#endif

	#ifdef GL_VERSION_4_3
		#define OPENGL_VERSION OPENGL_VERSION_4_3
	#endif

	#ifdef GL_VERSION_4_4
		#define OPENGL_VERSION OPENGL_VERSION_4_4
	#endif

	#ifdef GL_VERSION_4_5
		#define OPENGL_VERSION OPENGL_VERSION_4_5
	#endif

	#ifdef GL_VERSION_4_6
		#define OPENGL_VERSION OPENGL_VERSION_4_6
	#endif */

	// Desired OpenGL version. Note that the current device might not be able to load this version.
	#define OPENGL_VERSION OPENGL_VERSION_4_6
}