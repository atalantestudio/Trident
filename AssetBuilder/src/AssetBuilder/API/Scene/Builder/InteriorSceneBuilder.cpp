// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	InteriorSceneBuilder::InteriorSceneBuilder(std::filesystem::path rootOutputPath) :
		StaticSceneBuilder(rootOutputPath)
	{}

	void InteriorSceneBuilder::addRoom(const Room& room) {
		// TODO(asset-builder/scene/interior): Restore.
		/* for (const StaticSurface& surface : room.surfaces) {
			addSurface(surface);
		} */
	}

	void InteriorSceneBuilder::addFrame(const Frame& frame) {
		// TODO(asset-builder/scene/interior): Restore.
		/* for (const StaticSurface& surface : frame.surfaces) {
			addSurface(surface);
		} */
	}
}