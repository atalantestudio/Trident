#pragma once

#include "ProjectA/Renderer/VertexArray.hpp"

namespace ProjectA {
	class OpenGLVertexArray : public VertexArray {
		private:
			uint32_t id;
			std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
			std::shared_ptr<IndexBuffer> indexBuffer;

		public:
			OpenGLVertexArray();

			virtual ~OpenGLVertexArray();

			virtual void bind() const override;

			virtual void unbind() const override;

			virtual const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const override;

			virtual const std::shared_ptr<IndexBuffer>& getIndexBuffer() const override;

			virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;

			virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;
	};
}