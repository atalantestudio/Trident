#pragma once

#include <string>

namespace ProjectA {
	class Shader {
		private:
			uint32_t program;

		public:
			Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

			~Shader();

			void bind() const;

			void unbind() const;
	};
}