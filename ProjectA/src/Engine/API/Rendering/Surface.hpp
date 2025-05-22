#pragma once

#include "Engine/API/Rendering/ImageFormat.hpp"

namespace ProjectA {
	class Surface {
		public:
			virtual ImageFormat getFormat() const = 0;
	};
}