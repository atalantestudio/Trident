#include "pch.hpp"
#include "ShaderLibrary.hpp"

namespace ProjectA {
	Ref<Shader> ShaderLibrary::load(const std::string& path) {
		auto shader = Shader::create(path);

		add(shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& path, const std::string& name) {
		auto shader = Shader::create(path);

		add(shader, name);

		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name) const {
		CORE_ASSERT((shaders.find(name) != shaders.end()), "Could not find shader name \"{0}\" in the shader library.", name);

		return shaders.at(name);
	}

	void ShaderLibrary::add(const Ref<Shader>& shader) {
		add(shader, shader->getName());
	}

	void ShaderLibrary::add(const Ref<Shader>& shader, const std::string& name) {
		CORE_ASSERT((shaders.find(name) == shaders.end()), "Found duplicate shader name.");

		shaders[name] = shader;
	}
}