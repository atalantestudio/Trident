#pragma once

#include "ProjectA/Renderer/Shader.hpp"

// TODO: Temporary fix for glad/glad.h not usable in Sandbox
typedef unsigned int GLenum;

namespace ProjectA {
	class OpenGLShader : public Shader {
		private:
			uint32_t program;
			std::string name;

		private:
			std::string readFile(const std::string& path) const;

			std::unordered_map<GLenum, std::string> preprocess(const std::string& source) const;
			void compile(const std::unordered_map<GLenum, std::string>&);

		public:
			OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name);
			OpenGLShader(const std::string& path);

			virtual ~OpenGLShader();

			virtual void bind() const override;
			virtual void unbind() const override;

			virtual const std::string& getName() const override;

			virtual void setUniform(const std::string& name, int value) const override;
			virtual void setUniform(const std::string& name, float value) const override;
			virtual void setUniform(const std::string& name, const vec2<float>& value) const override;
			virtual void setUniform(const std::string& name, const vec3<float>& value) const override;
			virtual void setUniform(const std::string& name, const vec4<float>& value) const override;
			virtual void setUniform(const std::string& name, const mat4x4<float>& value) const override;
	};
}