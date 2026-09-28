// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Map/Builder/Build/Build.hpp"
#include "AssetBuilder/API/Map/Builder/Build/CreateFaceGeometries.hpp"
#include "AssetBuilder/API/Map/Builder/Build/EmissiveTriangleIndex.hpp"
#include "AssetBuilder/API/Map/Builder/Build/GenerateLightmaps.hpp"
#include "AssetBuilder/API/Map/Builder/Build/Triangle.hpp"
#include "AssetBuilder/API/Map/Builder/Build/TriangulateFaces.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CarveFace.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CreateFace.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CreatePoint.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CreatePointLight.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CreateRigidBody.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/CreateStaticProp.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/ExtrudeFace.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/LoadGeometry.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/LoadMaterial.hpp"
#include "AssetBuilder/API/Map/Builder/Operation/RemoveFace.hpp"
#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"
#include "AssetBuilder/Process/MaterialShader/DDGI.hpp"
#include "AssetBuilder/Process/MaterialShader/Lightmapped.hpp"
#include "AssetBuilder/Process/MaterialShader/LightmappedPBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBR.hpp"
#include "AssetBuilder/Process/MaterialShader/PBRDebug.hpp"

namespace Trident {
	ShaderModuleAsset MapBuilder::getMaterialShaderModuleAsset(uint32 identifier) {
		switch (identifier) {
			case 400: return LightmappedShaderModuleAsset();
			case 401: return LightmappedPBRShaderModuleAsset();
			case 402: return PBRShaderModuleAsset();
			case 403: return PBRDebugShaderModuleAsset();
			case 404: return DDGIShaderModuleAsset();
			default: {
				DEBUG_ASSERT(false);

				return {};
			}
		}
	}

	MapBuilder::MapBuilder(const MapDescriptor& descriptor) :
		descriptor(descriptor),
		facePositions(MAX_FACE_POSITIONS),
		faces(MAX_FACES)
	{
		if (descriptor.environmentMapImageAsset != nullptr) {
			ENGINE_ASSERT(descriptor.environmentMapImageAsset->isEligibleForCubeMap(), "The environment map is not eligible for a cube map.");
		}
	}

	Face& MapBuilder::getFace(uint32 index) {
		return faces[index];
	}

	uint32 MapBuilder::findMaterialShaderIndex(uint32 assetIdentifier) const {
		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

			if (assetIdentifier == materialShader.asset.identifier) {
				break;
			}

			materialShaderIndex += 1;
		}

		return materialShaderIndex;
	}

	uint32 MapBuilder::findMaterialGeometryIndex(const IntermediateMaterial& material, uint32 geometryIndex) const {
		uint32 materialGeometryIndex = 0;

		while (materialGeometryIndex < material.geometries.size()) {
			const IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];

			if (geometryIndex == materialGeometry.geometryIndex) {
				break;
			}

			materialGeometryIndex += 1;
		}

		return materialGeometryIndex;
	}
}