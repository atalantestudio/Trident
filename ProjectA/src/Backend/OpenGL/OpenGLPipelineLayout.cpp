#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		descriptorSetLayouts(descriptor.layouts)
	{
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLPipelineLayout]");
	}
}