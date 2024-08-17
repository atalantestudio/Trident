#pragma once

#include <string>

namespace ProjectA {
	class Shader {
		public:
			static Ref<Shader> create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name);
			static Ref<Shader> create(const std::string& path);

			virtual ~Shader() = default;

			virtual void bind() const = 0;
			virtual void unbind() const = 0;

			virtual const std::string& getName() const = 0;
	};
}