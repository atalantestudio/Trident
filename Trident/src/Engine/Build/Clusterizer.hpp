// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Build/ClusteredScene.hpp"

namespace Trident {
	ClusteredScene clusterizeScene(const Scene3D* scene);
}