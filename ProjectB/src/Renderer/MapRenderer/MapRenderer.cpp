// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Renderer/MapRenderer/MapRenderer.hpp"

namespace ProjectB {
	void MapRenderer::build() {
		renderPass = createRenderPass();
	}

	void MapRenderer::render(ref<CommandEncoder> commandEncoder) const {}

	void MapRenderer::postSetScene() {}

	void MapRenderer::postSetCameraController() {}

	void MapRenderer::loadMap(const Map& map) {
		// TODO(project-b/renderer/scene/load-map)
		BREAKPOINT();
	}
}