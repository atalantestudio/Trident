#pragma once

#include "Engine/Renderer/Buffer/Attribute.hpp"
#include "Engine/Renderer/Buffer/VertexAttribute.hpp"

namespace ProjectA {
	class BufferLayout {
		public:
			std::vector<VertexAttribute> vertexAttributes;
			uint32 stride = 0;

		public:
			template<typename T>
			void add(const uint32 binding, const bool normalized) {
				VertexAttribute vertexAttribute{};
				vertexAttribute.binding = binding;
				vertexAttribute.location = static_cast<uint32>(vertexAttributes.size());
				vertexAttribute.type = getVertexInputAttributeType<T>();
				vertexAttribute.normalized = normalized;
				vertexAttribute.offset = stride;

				vertexAttributes.emplace_back(vertexAttribute);
				stride += getAttributeSize<T>();
			}

			// TODO: Helper macro for iterators.
			std::vector<VertexAttribute>::const_iterator begin() const;
			std::vector<VertexAttribute>::iterator begin();

			std::vector<VertexAttribute>::const_iterator end() const;
			std::vector<VertexAttribute>::iterator end();
	};
}