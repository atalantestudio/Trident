#pragma once

namespace ProjectA {
	#define PROJECT_ROOT_DIRECTORY "/"

	// Verify the validity of the checksum for each table of a TTF file.
	// #define TTF_PARSER_VERIFY_CHECKSUM

	// For glyphs with a single contour, checks if the contour is counter-clockwise.
	// If so, inverts it before building the glyph map.
	#define TTF_FIX_INVERTED_CONTOURS

	// If enabled, OpenGL shaders will be sourced from SPIR-V binaries.
	// If disabled, they will be sourced from GLSL strings.
	#define OPENGL_USE_SPIRV
}