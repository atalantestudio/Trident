// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::ShaderModule::ShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) :
		Trident::ShaderModule(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

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
			&blob,
			#if defined(BUILD_MODE_DEBUG)
				&errorBlob
			#else
				nullptr
			#endif
		);

		#if defined(BUILD_MODE_DEBUG)
			if (errorBlob) {
				const view<char8> error(pointer_cast<const char8*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize());

				ADAPTER_ASSERT(false, "Failed to compile shader: []", error);

				errorBlob->Release();
			}
		#else
			ADAPTER_ASSERT(SUCCEEDED(result), "Failed to compile shader.");
		#endif
	}

	DX12::ShaderModule::~ShaderModule() {
		blob->Release();
	}
}