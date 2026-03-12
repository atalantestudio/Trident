// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Instance.hpp"
#include "Engine/Adapter/Graphics/GL/Surface.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Window/Window.hpp"
#include "Engine/Application/ApplicationProperties.hpp"

namespace Trident::GL {
	void bindAdapter(Trident::GraphicsAdapter* adapter);
}