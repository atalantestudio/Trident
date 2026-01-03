#pragma once

#include "Engine/API/Rendering/BlendFactor.hpp"

namespace ProjectA {
	static GLenum getGlBlendFactor(BlendFactor blendFactor) {
		switch (blendFactor) {
			case BlendFactor::ZERO:
				return GL_ZERO;
			case BlendFactor::ONE:
				return GL_ONE;
			case BlendFactor::SRC_COLOR:
				return GL_SRC_COLOR;
			case BlendFactor::ONE_MINUS_SRC_COLOR:
				return GL_ONE_MINUS_SRC_COLOR;
			case BlendFactor::DST_COLOR:
				return GL_DST_COLOR;
			case BlendFactor::ONE_MINUS_DST_COLOR:
				return GL_ONE_MINUS_DST_COLOR;
			case BlendFactor::SRC_ALPHA:
				return GL_SRC_ALPHA;
			case BlendFactor::ONE_MINUS_SRC_ALPHA:
				return GL_ONE_MINUS_SRC_ALPHA;
			case BlendFactor::DST_ALPHA:
				return GL_DST_ALPHA;
			case BlendFactor::ONE_MINUS_DST_ALPHA:
				return GL_ONE_MINUS_DST_ALPHA;
			case BlendFactor::CONSTANT_COLOR:
				return GL_CONSTANT_COLOR;
			case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
				return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendFactor::CONSTANT_ALPHA:
				return GL_CONSTANT_ALPHA;
			case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
				return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendFactor::SRC_ALPHA_SATURATE:
				return GL_SRC_ALPHA_SATURATE;
			case BlendFactor::SRC1_COLOR:
				return GL_SRC1_COLOR;
			case BlendFactor::ONE_MINUS_SRC1_COLOR:
				return GL_ONE_MINUS_SRC1_COLOR;
			case BlendFactor::SRC1_ALPHA:
				return GL_SRC1_ALPHA;
			case BlendFactor::ONE_MINUS_SRC1_ALPHA:
				return GL_ONE_MINUS_SRC1_ALPHA;
		}

		DEBUG_ASSERT(false);

		return 0;
	}
}