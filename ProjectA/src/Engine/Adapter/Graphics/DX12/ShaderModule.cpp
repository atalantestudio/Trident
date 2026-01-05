// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	DX12::ShaderModule::ShaderModule(view<char8> code, const ShaderModuleDescriptor& descriptor) :
		ProjectA::ShaderModule(descriptor)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		HRESULT result;
		UINT flags = 0;

		#ifdef BUILD_MODE_DEBUG
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

		result = D3DCompile(
			&code[0],
			code.count(),
			nullptr,
			nullptr,
			nullptr,
			&descriptor.entryPoint[0],
			target,
			flags,
			0,
			&blob,
			nullptr
		);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to compile shader.");
	}
}