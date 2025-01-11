#pragma once

#include "ProjectA/core.hpp"
#include "Texture.hpp"

namespace ProjectA {
	class Texture2D : public Texture {
		public:
			static Ref<Texture2D> create(std::size_t width, std::size_t height);
			static Ref<Texture2D> create(const std::string& path);
	};
}