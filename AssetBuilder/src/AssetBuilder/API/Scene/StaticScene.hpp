// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/BuildMaterialShader.hpp"

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"
#include "Engine/API/Scene/StaticVertex.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	struct BuildStaticMaterial3D {
		uint32 identifier;
		uint32 shaderIndex;
		range<uint32, uint32> indexRange;
	};

	struct StaticScene2D {
		sequence<StaticTriangle2D> triangles;
		sequence<svec2<float32>> vertices;
		sequence<svec2<float32>> textureCoordinates;
	};

	struct StaticScene3D {
		sequence<StaticVertex3D> indices;
		sequence<svec3<float32>> vertices;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<BuildMaterialShader> materialShaders;
		sequence<BuildStaticMaterial3D> materials;
		ImageAssetDescriptor lightmapAtlasImageAssetDescriptor;
	};
}