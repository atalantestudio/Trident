#pragma once

#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	class OpenGLVertexArray : public VertexArray {
		private:
			std::uint32_t id;
			std::vector<ref<VertexBuffer>> vertexBuffers;
			ref<IndexBuffer> indexBuffer;

		public:
			OpenGLVertexArray();

			virtual ~OpenGLVertexArray();

			virtual void bind() const override;

			virtual void unbind() const override;

			virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const override;

			virtual const ref<IndexBuffer>& getIndexBuffer() const override;

			virtual void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) override;

			virtual void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) override;
	};
}