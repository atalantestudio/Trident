// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsPipeline.hpp"

namespace Trident {
	class GL::GraphicsPipeline final : public Trident::GraphicsPipeline {
		public:
			struct DrawBuffer {
				GLboolean blend;
				GLenum srcColorBlendFactor;
				GLenum dstColorBlendFactor;
			};

		public:
			explicit GraphicsPipeline(const Trident::Device* device);

		public:
			GLuint program;
			GLuint vertexArray;
			GLenum primitiveTopology;
			GLenum frontFace;
			GLenum cullMode;
			GLenum polygonModeFace;
			GLenum polygonMode;
			GLfloat lineWidth;
			GLfloat minSampleShading;
			sequence<GLsizei> vertexInputBindingStrides;
			sequence<DrawBuffer> drawBuffers;
			GLboolean depthTest;
			GLboolean depthWrite;
			GLenum depthFunc;
			bool multisample;
			bool dynamicLineWidth;
			uint32 subpassIndex;
	};

	namespace GL {
		ref<Trident::GraphicsPipeline> createGraphicsPipeline(const Trident::Device* device, const GraphicsPipelineDescriptor& descriptor);

		void destroyGraphicsPipeline(const Trident::Device* device, const Trident::GraphicsPipeline* graphicsPipeline);
	}
}