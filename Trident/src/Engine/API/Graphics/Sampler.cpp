// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/Sampler.hpp"

namespace Trident {
	Sampler::Sampler(const Device* device) :
		device(device)
	{}

	Sampler::~Sampler() {
		device->adapter->destroySamplerImplementation(device, this);
	}
}