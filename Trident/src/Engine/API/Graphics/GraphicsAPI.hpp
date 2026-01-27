// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class GraphicsAPI : uint8 {
		// NOTE: Not supported.
		CPU,

		DX12,
		GL,
		VK,
	};

	// TODO: Implement.
	// API_QUEUE = Graphics API listed by priority.
	//             First APIs are more recent but more performant,
	//             last APIs are older but have better support

	// Define sint32 selectedApiIndex

	// Get "GraphicsAPI" setting (unsigned byte) from file
		// If no setting
			// Set selectedApiIndex = 0
		// Else if the setting is not in API_QUEUE
			// Emit warn "Unsupported graphics API. Using API 0. Updated the property in the file to avoid future warnings."
			// Set selectedApiIndex = 0
			// Overwrite file setting with selectedApiIndex
		// Else
			// Set selectedApiIndex = setting

	// Define queue apiTestQueue = API_QUEUE
		// Move the API at selectedApiIndex to the top

	// Define selectedApi
	// Define supportedApiIndices

	// For each API testedApi in API_QUEUE
		// Define available = testIfAvailable(testedApi)
		// If available
			// Push index of testedApi to supportedApiIndices

	// Define finalApiIndex = -1

	// For each API supportedApi in supportedApiIndices
		// If selectedApiIndex = index of supportedApi
			// Set finalApiIndex = index of supportedApi
			// Break

	// If finalApiIndex = -1
		// Emit warn "The selected API is not supported."
		// Set finalApiIndex = 0

	// Emit info "Using graphics adapter for []."
	// Define selectedApi = API_QUEUE[supportedApiIndices[finalApiIndex]]

	// Initialize selectedApi (these are examples).

	/* sint32 apiIndex = 0;
	sint32 apiTestQueue[5] = {
		77, // Vulkan
		63, // DirectX
		48, // Metal
		22, // WebGPU
		19, // OpenGL
	};

	{
		const auto api = apiTestQueue[apiIndex];

		for (sint32 i = apiIndex - 1; i >= 0; i--) {
			apiTestQueue[i + 1] = apiTestQueue[i];
		}

		apiTestQueue[0] = api;
	} */
}