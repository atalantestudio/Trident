#pragma once

#include <string>

namespace ProjectA {
	class Shader {
		public:
			static Ref<Shader> create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
			static Ref<Shader> create(const std::string& path);

			virtual ~Shader() = default;

			virtual void bind() const = 0;
			virtual void unbind() const = 0;
	};
}