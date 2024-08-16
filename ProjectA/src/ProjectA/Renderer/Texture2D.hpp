#pragma once

#include <string>
#include "ProjectA/core.hpp"
#include "Texture.hpp"

namespace ProjectA {
	class Texture2D : public Texture {
		public:
			static Ref<Texture2D> create(const std::string& path);
	};
}