#pragma once

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	// API-agnostic GUI renderer.
	class GUIRenderer : public Renderer {
		public:
			void render() const override;
	};
}