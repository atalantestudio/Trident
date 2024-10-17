#pragma once

#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	class OpenGLVertexArray : public VertexArray {
		private:
			std::uint32_t id;
			std::uint32_t vertexBufferIndex;
			std::vector<Ref<VertexBuffer>> vertexBuffers;
			Ref<IndexBuffer> indexBuffer;

		public:
			OpenGLVertexArray();

			virtual ~OpenGLVertexArray();

			virtual void bind() const override;

			virtual void unbind() const override;

			virtual const std::vector<Ref<VertexBuffer>>& getVertexBuffers() const override;

			virtual const Ref<IndexBuffer>& getIndexBuffer() const override;

			virtual void addVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

			virtual void setIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
	};
}