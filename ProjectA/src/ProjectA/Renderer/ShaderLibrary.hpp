#pragma once

#include "ProjectA/core.hpp"
#include "ProjectA/Renderer/Shader.hpp"

namespace ProjectA {
	class ShaderLibrary {
		private:
			std::unordered_map<std::string, ref<Shader>> shaders;

		public:
			ref<Shader> load(const std::string& path);
			ref<Shader> load(const std::string& path, const std::string& name);

			ref<Shader> get(const std::string& name) const;

			void add(const ref<Shader>& shader);
			void add(const ref<Shader>& shader, const std::string& name);
	};
}