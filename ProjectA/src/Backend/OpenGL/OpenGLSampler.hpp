#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"

namespace ProjectA {
	class OpenGLSampler : public Sampler, public ObjectWrapper<GLuint> {
		public:
			OpenGLSampler(const SamplerDescriptor& descriptor);

			~OpenGLSampler();
	};
}