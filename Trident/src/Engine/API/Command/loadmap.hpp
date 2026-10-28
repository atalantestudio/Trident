// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Application/Application.hpp"
#include "Engine/API/Command/Command.hpp"

namespace Trident {
	void loadmap(view<char8> path, Application* application);
}