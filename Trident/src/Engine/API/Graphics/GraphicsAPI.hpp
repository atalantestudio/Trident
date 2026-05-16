// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class GraphicsAPI : uint8 {
		// DirectX 12.
		DX12,

		// OpenGL.
		GL,

		// Vulkan.
		VK,

		count,
	};

	template<>
	inline view<char8> serialize(GraphicsAPI object) {
		static constexpr view<char8> GRAPHICS_API_NAMES[] = {
			"DirectX12",
			"OpenGL",
			"Vulkan",
		};

		DEBUG_ASSERT(object < GraphicsAPI::count);

		return GRAPHICS_API_NAMES[static_cast<uint8>(object)];
	}

	template<>
	inline GraphicsAPI deserialize(view<char8> text) {
		if (text == "DirectX12") {
			return GraphicsAPI::DX12;
		}

		if (text == "OpenGL") {
			return GraphicsAPI::GL;
		}

		if (text == "Vulkan") {
			return GraphicsAPI::VK;
		}

		ENGINE_ASSERT(false, "Deserialization to GraphicsAPI failed for text \"[]\".", text);

		return static_cast<GraphicsAPI>(0);
	}

	inline GraphicsAPI getDefaultGraphicsApi() {
		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			return GraphicsAPI::DX12;
		#elif OPERATING_SYSTEM == OPERATING_SYSTEM_LINUX
			return GraphicsAPI::VK;
		#else
			return GraphicsAPI::GL;
		#endif
	}
}