#pragma once

#include "Engine/Renderer/GUI/GUIRendererSettings.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	// This class is API-agnostic.
	class GUIRenderer : public Renderer {
		public:
			GUIRendererSettings settings;

		public:
			void render() const override;
	};
}