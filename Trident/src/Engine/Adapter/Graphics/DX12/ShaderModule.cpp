// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderTarget.hpp"
#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	DX12::ShaderModule::ShaderModule(const Trident::Device* device) :
		Trident::ShaderModule(device)
	{}

	ref<Trident::ShaderModule> DX12::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset) {
		ADAPTER_ASSERT(false, "Shader module assets are not currently supported by the DirectX 12 adapter.");

		return nullptr;
	}

	ref<Trident::ShaderModule> DX12::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, const sequence<uint8>& code) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<DX12::ShaderModule> shaderModule = allocateRef<DX12::ShaderModule>(device);
		shaderModule->code = code;
		shaderModule->bytecode.pShaderBytecode = &shaderModule->code[0];
		shaderModule->bytecode.BytecodeLength = shaderModule->code.count();

		return shaderModule;
	}

	void DX12::destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule) {}
}