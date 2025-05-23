#pragma once

#include "Backend/OpenGL/OpenGLPipelineLayout.hpp"

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class OpenGLRenderPipeline : public RenderPipeline {
		public:
			OpenGLRenderPipeline(const RenderPipelineDescriptor& descriptor);

			~OpenGLRenderPipeline();

		public:
			const OpenGLPipelineLayout& layout;

			GLuint vertexArray;
			const GLuint program;
			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
			const GLenum polygonModeFace;
			const GLenum polygonMode;
			const GLfloat lineWidth;
			const GLfloat minSampleShading;
			const GLboolean depthTest;
			const GLboolean depthWrite;
			const GLenum depthFunc;
			const bool dynamicLineWidth;
	};
}