// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/GraphicsPipeline.hpp"

namespace Trident {
	class GL::GraphicsPipeline final : public Trident::GraphicsPipeline {
		public:
			struct DrawBuffer {
				GLboolean blend;
				GLenum srcColorBlendFactor;
				GLenum dstColorBlendFactor;
			};

		public:
			explicit GraphicsPipeline(const GraphicsPipelineDescriptor& descriptor);

			~GraphicsPipeline() final override;

		public:
			const GLuint program;
			GLuint vertexArray;

			const GLenum primitiveTopology;
			const GLenum frontFace;
			const GLenum cullMode;
			const GLenum polygonModeFace;
			const GLenum polygonMode;
			const GLfloat lineWidth;
			const GLfloat minSampleShading;

			sequence<DrawBuffer> drawBuffers;

			const GLboolean depthTest;
			const GLboolean depthWrite;
			const GLenum depthFunc;

			const bool multisample;
			const bool dynamicLineWidth;

			const uint32 subpassIndex;
	};
}