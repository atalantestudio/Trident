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
				pathString += ".hlsl";

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

		const sequence<char8> code = Filesystem::openTextBuffer(&path[0]);

		return device->createShaderModule(descriptor, view<char8>(&code[0], code.count()));
	}

	ref<ShaderModule> Lightmapper::loadCompiledShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> path) const {
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

		const sequence<char8> code = Filesystem::openTextBuffer(&path[0]);

		return device->createShaderModule(descriptor, view<char8>(&code[0], code.count()));
	}
}