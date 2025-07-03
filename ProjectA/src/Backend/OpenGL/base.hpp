#pragma once

#include <GL/glcorearb.h>

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include <gl/GL.h>
#endif

#include <GL/glext.h>

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include <GL/wglext.h>
#endif

#ifndef OPENGL_USE_SPIRV
	// NOTE: <regex> is used for custom entrypoints within a GLSL shader.
	#include <regex>
#endif

#include "Backend/OpenGL/procedures.hpp"