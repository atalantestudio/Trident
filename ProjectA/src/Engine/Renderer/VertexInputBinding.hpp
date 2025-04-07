#pragma once

#include "Engine/Renderer/Buffer/Attribute.hpp"
#include "Engine/Renderer/Buffer/VertexInputAttributeType.hpp"
#include "Engine/Renderer/InputRate.hpp"
#include "Engine/Renderer/VertexInputAttribute.hpp"

namespace ProjectA {
	class VertexInputBinding {
		public:
			VertexInputBinding(InputRate inputRate);

			InputRate getInputRate() const;

			const chain<VertexInputAttribute>& getAttributes() const;

			uint32 getStride() const;

			template<typename T>
			void registerAttribute(bool normalized) {
				VertexInputAttribute vertexInputAttribute{};
				vertexInputAttribute.location = static_cast<uint32>(attributes.size());
				vertexInputAttribute.type = getVertexInputAttributeType<T>();
				vertexInputAttribute.normalized = normalized;
				vertexInputAttribute.offset = stride;

				attributes.emplace_back(vertexInputAttribute);

				stride += getAttributeSize<T>();
			}

			template<>
			void registerAttribute<mat3x3<float32>>(bool normalized) {
				registerAttribute<vec3<float32>>(normalized);
				registerAttribute<vec3<float32>>(normalized);
				registerAttribute<vec3<float32>>(normalized);
			}

			template<>
			void registerAttribute<mat3x3<float64>>(bool normalized) {
				registerAttribute<vec3<float64>>(normalized);
				registerAttribute<vec3<float64>>(normalized);
				registerAttribute<vec3<float64>>(normalized);
			}

			template<>
			void registerAttribute<mat4x4<float32>>(bool normalized) {
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
				registerAttribute<vec4<float32>>(normalized);
			}

			template<>
			void registerAttribute<mat4x4<float64>>(bool normalized) {
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
				registerAttribute<vec4<float64>>(normalized);
			}

		private:
			InputRate inputRate;
			chain<VertexInputAttribute> attributes;
			uint32 stride = 0;
	};
}