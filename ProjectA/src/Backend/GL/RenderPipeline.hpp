#pragma once

#include "Backend/GL/DrawBuffer.hpp"

#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline final : public RenderPipeline {
		public:
			explicit OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline() final override;

		public:
			const GLuint program;
			GLuint vaobj;

			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
			const GLenum polygonModeFace;
			const GLenum polygonMode;
			const GLfloat lineWidth;
			const GLfloat minSampleShading;

			sequence<OpenGLDrawBuffer> drawBuffers;

			const GLboolean depthTest;
			const GLboolean depthWrite;
			const GLenum depthFunc;

			const bool dynamicLineWidth;

			const uint32 subpassIndex;
	};
}