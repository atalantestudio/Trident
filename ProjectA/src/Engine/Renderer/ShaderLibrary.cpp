#include "pch.hpp"

#include "Engine/Renderer/ShaderLibrary.hpp"

namespace ProjectA {
	/*ref<Shader> ShaderLibrary::load(const std::string& path) {
		auto shader = Shader::create(path);

		add(shader);

		return shader;
	}

	ref<Shader> ShaderLibrary::load(const std::string& path, const std::string& name) {
		auto shader = Shader::create(path);

		add(shader, name);

		return shader;
	}

	ref<Shader> ShaderLibrary::get(const std::string& name) const {
		ENGINE_ASSERT((shaders.find(name) != shaders.end()), "Could not find shader name \"{0}\" in the shader library.", name);

		return shaders.at(name);
	}

	void ShaderLibrary::add(const ref<Shader>& shader) {
		add(shader, shader->getName());
	}

	void ShaderLibrary::add(const ref<Shader>& shader, const std::string& name) {
		ENGINE_ASSERT((shaders.find(name) == shaders.end()), "Found duplicate shader name.");

		shaders[name] = shader;
	}*/
}