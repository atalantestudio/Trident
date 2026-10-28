// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Command/loadmap.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	void loadmap(view<char8> path) {
		AssetLibrary assetLibrary(Filesystem::getProjectRootDirectory());

		// TODO(console/command/loadmap)
		ENGINE_ERROR_UNIMPLEMENTED();

		/* const ref<Scene3DAsset> sceneAsset = assetLibrary.getAsset<Scene3DAsset>(path);
		const ref<Scene3D> scene = allocateRef<Scene3D>(sceneAsset);

		renderer->setScene(scene); */
	}
}