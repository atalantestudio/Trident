// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createWallScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		Geometry3DDescriptor geometryDescriptor{};
		geometryDescriptor.vertexPositionComponents = {
			-0.5f,  0.5f,  0.0f,
			 0.5f,  0.5f,  0.0f,
			 0.5f, -0.5f,  0.0f,
			-0.5f, -0.5f,  0.0f,
		};
		geometryDescriptor.vertexPositionIndices = {
			0, 1, 2,
			0, 2, 3,
		};
		geometryDescriptor.vertexNormalComponents = {
			0.0f,  0.0f, -1.0f,
		};
		geometryDescriptor.vertexNormalIndices = {
			0, 0, 0,
			0, 0, 0,
		};
		geometryDescriptor.vertexTextureCoordinateComponents = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};
		geometryDescriptor.vertexTextureCoordinateIndices = {
			0, 1, 2,
			0, 2, 3,
		};

		const ref<Geometry3D> geometry = createRef<Geometry3D>(geometryDescriptor);
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Material pbrMaterial{};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterial.shaderModulePath = "Material/PBR.frag";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterial.shaderModulePath = "Material/PBRSingleSet.frag";

				break;
			}
		}

		pbrMaterial.descriptorSetLayoutBindings = {};

		const uint32 pbrMaterialIndex = scene.registerMaterial(pbrMaterial);

		MaterialReference pbrMaterialReference{};
		pbrMaterialReference.materialIndex = pbrMaterialIndex;
		pbrMaterialReference.descriptorImagePaths = {};

		const uint32 pbrMaterialReferenceIndex = scene.registerMaterialReference(pbrMaterialReference);

		/* SceneInstanceDescriptor instanceDescriptor{};
		instanceDescriptor.position = {0.0_m, 0.0_m, 0.4_m};
		instanceDescriptor.rotation = {0.0_rad, 0.0_rad, 0.0_rad};
		instanceDescriptor.geometryIndex = geometryIndex;
		instanceDescriptor.materialReferenceIndex = pbrMaterialReferenceIndex;

		scene.registerInstance(instanceDescriptor); */

		return scene;
	}
}