// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Lightmapper/Lightmapper.hpp"

namespace Trident {
	ref<ShaderModule> Lightmapper::loadShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const {
		std::string pathString = "Asset/Shader/";
		pathString += path;

		switch (adapter->getApi()) {
			case GraphicsAPI::DX12: {
				pathString += ".dxil";

				break;
			}
			case GraphicsAPI::VK: {
				pathString += ".spv";

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		path = view<char8>(&pathString[0], pathString.size());

		const sequence<uint8> code = Filesystem::openByteBuffer(path);

		return device->createShaderModule(descriptor, code);
	}
}