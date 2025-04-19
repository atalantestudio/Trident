#include "pch.hpp"

#include "Engine/Renderer/FrontFace.hpp"

namespace ProjectA {
	static GLenum getGlFrontFace(FrontFace frontFace) {
		static constexpr GLenum glFrontFaces[] = {
			GL_CW,
			GL_CCW,
		};

		return glFrontFaces[static_cast<uint8>(frontFace)];
	}
}