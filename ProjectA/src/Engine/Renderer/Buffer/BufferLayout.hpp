#pragma once

#include "Engine/Renderer/Buffer/Attribute.hpp"
#include "Engine/Renderer/Buffer/VertexAttribute.hpp"
#include "Engine/Renderer/Buffer/VertexBinding.hpp"

namespace ProjectA {
	class BufferLayout {
		public:
			std::vector<VertexBinding> vertexBindings;
			std::vector<VertexAttribute> vertexAttributes;

		public:
			void defineBinding(const uint32 index, const InputRate inputRate);

			template<typename T>
			void add(const uint32 bindingIndex, const bool normalized) {
				VertexAttribute vertexAttribute{};
				vertexAttribute.binding = bindingIndex;
				vertexAttribute.location = static_cast<uint32>(vertexAttributes.size());
				vertexAttribute.type = getVertexInputAttributeType<T>();
				vertexAttribute.normalized = normalized;
				vertexAttribute.offset = vertexBindings[bindingIndex].stride;

				vertexAttributes.emplace_back(vertexAttribute);

				// TODO: Check that the vertex binding exists.
				vertexBindings[bindingIndex].stride += getAttributeSize<T>();
			}

			template<>
			void add<mat3x3<float32>>(const uint32 bindingIndex, const bool normalized) {
				for (uint8 i = 0; i < 3; i++) {
					add<vec3<float32>>(bindingIndex, normalized);
				}
			}

			template<>
			void add<mat3x3<float64>>(const uint32 bindingIndex, const bool normalized) {
				for (uint8 i = 0; i < 3; i++) {
					add<vec3<float64>>(bindingIndex, normalized);
				}
			}

			template<>
			void add<mat4x4<float32>>(const uint32 bindingIndex, const bool normalized) {
				for (uint8 i = 0; i < 4; i++) {
					add<vec4<float32>>(bindingIndex, normalized);
				}
			}

			template<>
			void add<mat4x4<float64>>(const uint32 bindingIndex, const bool normalized) {
				for (uint8 i = 0; i < 4; i++) {
					add<vec4<float64>>(bindingIndex, normalized);
				}
			}
	};
}