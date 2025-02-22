#pragma once

#include <glad/glad.h>

#include "ProjectA/Renderer/Shader.hpp"

namespace ProjectA {
	class OpenGLShader : public Shader {
		private:
			uint32 program;
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

			virtual void setUniform(const std::string& name, int32 value) const override;
			virtual void setUniform(const std::string& name, const int32* value, uint32 count) const override;
			virtual void setUniform(const std::string& name, float32 value) const override;
			virtual void setUniform(const std::string& name, const vec2<float32>& value) const override;
			virtual void setUniform(const std::string& name, const vec3<float32>& value) const override;
			virtual void setUniform(const std::string& name, const vec4<float32>& value) const override;
			virtual void setUniform(const std::string& name, const mat4x4<float32>& value) const override;
	};
}