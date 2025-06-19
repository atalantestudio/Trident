#include "pch.hpp"

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		PipelineLayout(descriptor),
		descriptorSetLayouts(descriptor.layouts)
	{}
}