#include "pch.hpp"

#include "Backend/OpenGL/OpenGLBuffer.hpp"
#include "Backend/OpenGL/OpenGLImageView.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"
#include "Backend/OpenGL/OpenGLSampler.hpp"

#include "Backend/OpenGL/DescriptorType.hpp"

namespace ProjectA {
	OpenGLPipelineLayout::OpenGLPipelineLayout(const PipelineLayoutDescriptor& descriptor) :
		descriptorSetLayouts(descriptor.layouts)
	{}
}