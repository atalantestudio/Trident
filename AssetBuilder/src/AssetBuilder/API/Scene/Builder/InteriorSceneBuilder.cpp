// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"
#include "AssetBuilder/API/Scene/Interior/Frame/DoorFrame.hpp"
#include "AssetBuilder/API/Scene/Interior/Room/RectangularRoom.hpp"

namespace Trident {
	InteriorSceneBuilder::InteriorSceneBuilder(std::filesystem::path rootOutputPath) :
		StaticSceneBuilder(rootOutputPath)
	{}
}