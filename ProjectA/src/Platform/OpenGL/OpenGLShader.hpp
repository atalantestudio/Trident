#pragma once

#include "math/mat/mat4x4.hpp"
#include "math/vec/vec4.hpp"
#include "ProjectA/Renderer/Shader.hpp"

/// TODO: Temporary fix for glad/glad.h not usable in Sandbox
typedef unsigned int GLenum;

namespace ProjectA {
	class OpenGLShader : public Shader {
		private:
			uint32_t program;

		private:
			std::string readFile(const std::string& path) const;

			std::unordered_map<GLenum, std::string> preprocess(const std::string& source) const;
			void compile(const std::unordered_map<GLenum, std::string>&);

		public:
			OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
			OpenGLShader(const std::string& path);

			virtual ~OpenGLShader();

			virtual void bind() const override;
			virtual void unbind() const override;

			void uploadUniform(const std::string& name, int) const;
			void uploadUniform(const std::string& name, float) const;
			void uploadUniform(const std::string& name, const vec3<float>&) const;
			void uploadUniform(const std::string& name, const vec4<float>&) const;
			void uploadUniform(const std::string& name, const mat4x4<float>&) const;
	};
}