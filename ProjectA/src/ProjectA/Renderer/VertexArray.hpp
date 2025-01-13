#pragma once

#include "ProjectA/Renderer/IndexBuffer.hpp"
#include "ProjectA/Renderer/VertexBuffer.hpp"

namespace ProjectA {
	class VertexArray {
		public:
			static ref<VertexArray> create();

			virtual ~VertexArray();

			virtual void bind() const = 0;

			virtual void unbind() const = 0;

			virtual const std::vector<ref<VertexBuffer>>& getVertexBuffers() const = 0;

			virtual const ref<IndexBuffer>& getIndexBuffer() const = 0;

			virtual void addVertexBuffer(const ref<VertexBuffer>& vertexBuffer) = 0;

			virtual void setIndexBuffer(const ref<IndexBuffer>& indexBuffer) = 0;
	};
}