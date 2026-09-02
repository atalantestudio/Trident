// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Surface.hpp"

namespace Trident {
	Surface::Surface(const Instance* instance) :
		instance(instance)
	{}

	Surface::~Surface() {
		instance->adapter->destroySurfaceImplementation(instance, this);
	}

	ImageFormat Surface::getFormat() const {
		return format;
	}
}