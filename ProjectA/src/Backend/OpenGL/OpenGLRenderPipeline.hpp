#pragma once

#include "Backend/OpenGL/OpenGLDrawBuffer.hpp"
#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline final : public RenderPipeline {
		public:
			explicit OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline() final override;

		public:
			const OpenGLPipelineLayout& layout;

			const GLuint program;
			GLuint vaobj;

			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
			const GLenum polygonModeFace;
			const GLenum polygonMode;
			const GLfloat lineWidth;
			const GLfloat minSampleShading;

			std::vector<OpenGLDrawBuffer> drawBuffers;

			const GLboolean depthTest;
			const GLboolean depthWrite;
			const GLenum depthFunc;

			const bool dynamicLineWidth;

			const uint32 subpassIndex;
	};
}