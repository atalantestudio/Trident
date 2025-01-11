#pragma once

#include "pch.hpp"

#include "ProjectA/core.hpp"

namespace ProjectA {
	class Shader {
		public:
			static Ref<Shader> create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name);
			static Ref<Shader> create(const std::string& path);

			virtual ~Shader() = default;

			virtual void bind() const = 0;
			virtual void unbind() const = 0;

			virtual void setUniform(const std::string& name, int value) const = 0;
			virtual void setUniform(const std::string& name, float value) const = 0;
			virtual void setUniform(const std::string& name, const vec2<float>& value) const = 0;
			virtual void setUniform(const std::string& name, const vec3<float>& value) const = 0;
			virtual void setUniform(const std::string& name, const vec4<float>& value) const = 0;
			virtual void setUniform(const std::string& name, const mat4x4<float>& value) const = 0;

			virtual const std::string& getName() const = 0;
	};
}