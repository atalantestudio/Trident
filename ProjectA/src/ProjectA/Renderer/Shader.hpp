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

			virtual void setUniform(const std::string& name, std::int32_t value) const = 0;
			virtual void setUniform(const std::string& name, const std::int32_t* value, std::uint32_t count) const = 0;
			virtual void setUniform(const std::string& name, std::float_t value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec2<std::float_t>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec3<std::float_t>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::vec4<std::float_t>& value) const = 0;
			virtual void setUniform(const std::string& name, const lyah::mat4x4<std::float_t>& value) const = 0;

			virtual const std::string& getName() const = 0;
	};
}