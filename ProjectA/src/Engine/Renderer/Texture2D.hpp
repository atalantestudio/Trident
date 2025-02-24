#pragma once

#include "Engine/base.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace ProjectA {
	class Texture2D : public Texture {
		public:
			static ref<Texture2D> create(std::size_t width, std::size_t height);
			static ref<Texture2D> create(const std::string& path);
	};
}