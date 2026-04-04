// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createBoxScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		const vec3<float32> size = {1.0_m, 1.0_m, 1.0_m};
		const ref<Geometry3D> geometry = createRef<BoxGeometry3D>(size);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Shader defaultMaterialShader = {
			.descriptorSetLayoutBindings = {},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				defaultMaterialShader.modulePath = "Visibility/Material/Default";

				break;
			}
			case GraphicsAPI::GL: {
				defaultMaterialShader.modulePath = "Visibility/Material/DefaultGL";

				break;
			}
		}

		const uint32 defaultMaterialShaderIndex = scene.registerMaterialShader(defaultMaterialShader);

		const uint32 defaultMaterialIndex = scene.registerMaterial({
			.shaderIndex = defaultMaterialShaderIndex,
			.writeDescriptorSetBindings = {},
		});

		Shader testMaterialShader = {
			.descriptorSetLayoutBindings = {},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				testMaterialShader.modulePath = "Visibility/Material/Default";

				break;
			}
			case GraphicsAPI::GL: {
				testMaterialShader.modulePath = "Visibility/Material/DefaultGL";

				break;
			}
		}

		const uint32 testMaterialShaderIndex = scene.registerMaterialShader(testMaterialShader);

		const uint32 testMaterialIndex = scene.registerMaterial({
			.shaderIndex = testMaterialShaderIndex,
			.writeDescriptorSetBindings = {},
		});

		scene.registerInstance({
			.position = {0.0_m, 0.0_m, 3.0_m},
			.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = geometryIndex,
			.materialIndex = testMaterialIndex,
		});

		return scene;
	}
}