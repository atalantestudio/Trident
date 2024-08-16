#pragma once

#include <string>

namespace ProjectA {
	class Shader {
		public:
			static Shader* create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

			virtual ~Shader() = default;

			virtual void bind() const = 0;
			virtual void unbind() const = 0;
	};
}