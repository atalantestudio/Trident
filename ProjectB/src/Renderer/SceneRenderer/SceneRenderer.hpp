// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace ProjectB {
	using namespace Trident;

	// TODO(project-b/renderer/scene)
	class SceneRenderer : public Trident::SceneRenderer<Scene3D> {
		public:
			void build() final override;

			void render(ref<CommandEncoder> commandEncoder) const final override;

			void postSetScene() final override;

			void postSetCameraController() final override;
	};
}