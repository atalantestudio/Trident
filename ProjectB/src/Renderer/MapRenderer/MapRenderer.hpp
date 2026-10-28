// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Map/Map.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Renderer/3D/SceneRenderer.hpp"

namespace ProjectB {
	using namespace Trident;

	// TODO(project-b/renderer/scene)
	class MapRenderer : public SceneRenderer<Scene3D> {
		private:
			static constexpr ImageSampleCount SAMPLE_COUNT = ImageSampleCount::x1;

		public:
			void build() override;

			void render(ref<CommandEncoder> commandEncoder) const override;

			void postSetScene() override;

			void postSetCameraController() override;

			void loadMap(const Map& map) override;

		private:
			ref<RenderPass> createRenderPass() const;

		private:
			ref<RenderPass> renderPass;
	};
}