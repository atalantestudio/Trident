#pragma once

#include "ProjectA/core.hpp"
#include "Shader.hpp"

namespace ProjectA {
	class ShaderLibrary {
		private:
			std::unordered_map<std::string, Ref<Shader>> shaders;

		public:
			Ref<Shader> load(const std::string& path);
			Ref<Shader> load(const std::string& path, const std::string& name);

			Ref<Shader> get(const std::string& name) const;

			void add(const Ref<Shader>& shader);
			void add(const Ref<Shader>& shader, const std::string& name);
	};
}