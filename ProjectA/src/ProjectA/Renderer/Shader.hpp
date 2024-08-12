#pragma once

#include <string>
#include "math/mat/mat4x4.hpp"
#include "math/vec/vec4.hpp"

namespace ProjectA {
	class Shader {
		private:
			uint32_t program;

		public:
			Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

			~Shader();

			void bind() const;

			void unbind() const;

			void uploadUniform(const std::string& name, const vec4<float>& vector) const;

			void uploadUniform(const std::string& name, const mat4x4<float>& matrix) const;
	};
}