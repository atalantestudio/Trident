#pragma once

#include "pch.hpp"

#include "ProjectA/core.hpp"

namespace ProjectA {
	class Shader {
		public:
			static ref<Shader> create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name);
			static ref<Shader> create(const std::string& path);

			virtual ~Shader() = default;

			virtual void bind() const = 0;
			virtual void unbind() const = 0;

			virtual void setUniform(const std::string& name, int32 value) const = 0;
			virtual void setUniform(const std::string& name, const int32* value, uint32 count) const = 0;
			virtual void setUniform(const std::string& name, float32 value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec2<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec3<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec4<float32>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::mat4x4<float32>& value) const = 0;

			virtual const std::string& getName() const = 0;
	};
}