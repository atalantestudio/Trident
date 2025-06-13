#pragma once

namespace ProjectA {
	#define GPU_TYPE_DISCRETE 0
	#define GPU_TYPE_INTEGRATED 1

	// The physical device preferred by the application.
	// There is no guarantee that it will be selected at runtime.
	#define PREFERRED_GPU_TYPE GPU_TYPE_DISCRETE

	#define PROJECT_ROOT_DIRECTORY "/"

	// TODO: Implement.
	// Verify the validity of the checksum for each table of a TTF file.
	// #define TTF_PARSER_VERIFY_CHECKSUM

	// For glyphs with a single contour, checks if the contour is counter-clockwise.
	// If so, inverts it before building the glyph map.
	#define TTF_FIX_INVERTED_CONTOURS

	// TODO: Refactor the function loader to not load DSA procedures if this is disabled.
	// If enabled, the OpenGL adapter will use Direct State Access procedures (if available).
	// If disabled, the OpenGL adapter will use standard procedures.
	#define OPENGL_USE_DSA

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define OPENGL_USE_SPIRV
}