#pragma once

#include "Engine/API/Rendering/Surface.hpp"

namespace ProjectA {
	class OpenGLSurface : public Surface {
		public:
			ImageFormat getFormat() const final override;
	};
}