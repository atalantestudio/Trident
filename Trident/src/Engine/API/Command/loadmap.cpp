// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Command/loadmap.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	void loadmap(view<char8> path, Application* application) {
		Renderer* renderer = application->getMainRenderer();

		ENGINE_ASSERT(renderer != nullptr, "No main renderer defined.");

		AssetLibrary assetLibrary(Filesystem::getProjectRootDirectory());

		const Map map = assetLibrary.loadMap(path);

		renderer->loadMap(map);
	}
}