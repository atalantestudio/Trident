// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::ShaderModule::ShaderModule(const Trident::Device* device) :
		Trident::ShaderModule(device)
	{}

	ref<Trident::ShaderModule> DX12::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> shaderModuleAsset) {
		ADAPTER_ASSERT(false, "Shader module assets are not supported by the DirectX 12 adapter.");

		return nullptr;
	}

	ref<Trident::ShaderModule> DX12::createShaderModule(const Trident::Device* device, const ShaderModuleDescriptor& descriptor, view<char8> code) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<DX12::ShaderModule> shaderModule = createRef<DX12::ShaderModule>(device);
		HRESULT result;
		UINT flags = 0;

		#if defined(BUILD_MODE_DEBUG)
			flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		#endif

		LPCSTR target;

		switch (descriptor.stage) {
			case ShaderStage::VERTEX: {
				target = "vs_5_0";

				break;
			}
			case ShaderStage::FRAGMENT: {
				target = "ps_5_0";

				break;
			}
			case ShaderStage::COMPUTE: {
				target = "cs_5_0";

				break;
			}
		}

		#if defined(BUILD_MODE_DEBUG)
			ID3DBlob* errorBlob;
		#endif

		result = D3DCompile(
			&code[0],
			code.count(),
			#if defined(BUILD_MODE_DEBUG)
				&descriptor.name[0],
			#else
				nullptr,
			#endif
			nullptr,
			nullptr,
			&descriptor.entryPoint[0],
			target,
			flags,
			0,
			&shaderModule->blob,
			#if defined(BUILD_MODE_DEBUG)
				&errorBlob
			#else
				nullptr
			#endif
		);

		#if defined(DEBUG_OBJECT_NAME)
			if (errorBlob) {
				const view<char8> error(pointer_cast<const char8*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

				ADAPTER_ASSERT(false, "Failed to compile shader: []", error);

				errorBlob->Release();
			}
		#else
			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to compile shader.");
		#endif

		return shaderModule;
	}

	void DX12::destroyShaderModule(const Trident::Device* device, const Trident::ShaderModule* shaderModule) {
		const DX12::ShaderModule* _shaderModule = static_cast<const DX12::ShaderModule*>(shaderModule);

		_shaderModule->blob->Release();
	}
}