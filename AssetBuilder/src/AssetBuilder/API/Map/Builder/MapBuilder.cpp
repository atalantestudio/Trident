// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	MapBuilder::MapBuilder(const MapDescriptor& descriptor) :
		descriptor(descriptor),
		facePositions(MAX_FACE_POSITIONS),
		indices(MAX_INDICES),
		faces(MAX_FACES)
	{}

	Face& MapBuilder::getFace(uint32 index) {
		return faces[index];
	}

	Face& MapBuilder::createFace(view<FaceVertex> vertices, svec3<float32> normal) {
		ENGINE_ASSERT(vertices.count() >= 3, "Not enough vertices.");

		Face face(this);
		face.vertexCount = static_cast<uint32>(vertices.count());
		face.indexRange.offset = faceIndexCount;
		face.normal = normal;

		uint32 vertexIndex = 0;

		while (vertexIndex < face.vertexCount) {
			const FaceVertex& vertex = vertices[vertexIndex];

			facePositions[facePositionCount + vertexIndex] = vertex.position;
			indices[faceIndexCount + vertexIndex] = facePositionCount + vertexIndex;

			vertexIndex += 1;
		}

		faces[faceCount] = face;

		vertexCount += face.vertexCount;
		triangleCount += face.vertexCount - 2;
		faceCount += 1;

		facePositionCount += face.vertexCount;
		faceIndexCount += face.vertexCount;

		return faces[faceCount - 1];
	}

	void MapBuilder::extrudeFace(Face& face, float32 scale) {
		if (abs(scale) < 1e-4) {
			return;
		}

		vec3<float32> a = face.getPosition(0);

		uint32 indexA = 0;

		while (indexA < face.vertexCount) {
			// TODO(lyah/scalar)
			facePositions[facePositionCount + indexA] = svec3<float32>(a) + face.normal * scale;

			const uint32 indexB = (indexA + 1) % face.vertexCount;
			const vec3<float32> b = face.getPosition(indexB);

			Face extrudedFace(this);
			extrudedFace.vertexCount = 4;
			extrudedFace.indexRange.offset = faceIndexCount + indexA * 4;

			// TODO(lyah/scalar)
			extrudedFace.normal = svec3<float32>(normalized(cross(b - a, face.normal * scale)));

			indices[extrudedFace.indexRange.offset + 0] = facePositionCount + indexB;
			indices[extrudedFace.indexRange.offset + 1] = facePositionCount + indexA;
			indices[extrudedFace.indexRange.offset + 2] = indices[face.indexRange.offset + indexA];
			indices[extrudedFace.indexRange.offset + 3] = indices[face.indexRange.offset + indexB];

			faces[faceCount] = extrudedFace;

			a = b;

			faceCount += 1;
			indexA += 1;
		}

		indexA = 0;

		while (indexA < face.vertexCount) {
			indices[face.indexRange.offset + indexA] = facePositionCount + indexA;

			indexA += 1;
		}

		facePositionCount += face.vertexCount;
		faceIndexCount += face.vertexCount * 4;
		vertexCount += face.vertexCount * 4;
		triangleCount += face.vertexCount * 2;
	}

	BuildScene3D MapBuilder::build() {
		BuildScene3D scene{};
		scene.skyColor = {0.0f, 0.0f, 0.0f};

		if (descriptor.skyboxImageAsset != nullptr) {
			scene.skyboxAssetIdentifier = descriptor.skyboxImageAsset->identifier;
		}
		else {
			scene.skyboxAssetIdentifier = max<uint32>();
		}

		scene.sunDirection = {0.0f, 0.0f, 0.0f};
		scene.sunColor = {0.0f, 0.0f, 0.0f};
		scene.geometries = {};
		scene.materialGeometries = {};
		scene.materials = {};
		scene.vertexCount = vertexCount;
		scene.triangleCount = triangleCount;
		scene.staticGeometry.vertexCount = vertexCount;
		scene.staticGeometry.lineIndexCount = vertexCount * 2;
		scene.staticGeometry.bakedVertices = sequence<BuildVertex3D>(vertexCount);

		const uint32 triangulatedIndexCount = triangleCount * 3;

		scene.staticGeometry.bakedIndices = sequence<uint32>(triangulatedIndexCount);
		scene.staticGeometry.lineIndices = sequence<uint32>(scene.staticGeometry.lineIndexCount);

		uint32 faceIndex = 0;
		uint32 vertexOffset = 0;
		uint32 indexOffset = 0;
		uint32 lineIndexOffset = 0;

		while (faceIndex < faceCount) {
			const Face& face = faces[faceIndex];
			const uint32 triangleCount = face.vertexCount - 2;

			uint32 vertexIndex = 0;
			uint32 triangleIndex = 0;

			while (vertexIndex < face.vertexCount) {
				BuildVertex3D& vertex = scene.staticGeometry.bakedVertices[vertexOffset + vertexIndex];
				vertex.position = facePositions[indices[face.indexRange.offset + vertexIndex]];
				vertex.normal = face.normal;

				// TODO(asset-builder/map): Calculate face tangent and vertex texture coordinates.
				vertex.tangent = {0.0f, 0.0f, 0.0f, 0.0f};
				vertex.textureCoordinate = {0.0f, 0.0f};

				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 0] = face.indexRange.offset + vertexIndex;
				scene.staticGeometry.lineIndices[lineIndexOffset + vertexIndex * 2 + 1] = face.indexRange.offset + (vertexIndex + 1) % face.vertexCount;

				vertexIndex += 1;
			}

			while (triangleIndex < triangleCount) {
				// TODO(asset-builder/map): Faces are assumed to be convex and have their vertices laid out in a clockwise order.
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 0] = face.indexRange.offset;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 1] = face.indexRange.offset + triangleIndex + 1;
				scene.staticGeometry.bakedIndices[indexOffset + triangleIndex * 3 + 2] = face.indexRange.offset + triangleIndex + 2;

				triangleIndex += 1;
			}

			vertexOffset += vertexIndex;
			indexOffset += triangleCount * 3;
			lineIndexOffset += face.vertexCount * 2;
			faceIndex += 1;
		}

		scene.staticGeometry.lightmapCoordinates = {};
		scene.staticGeometry.instances = {};
		scene.staticGeometry.materialShaders = {};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.identifier = AssetLibrary::createAssetIdentifier();
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.version = {0, 0, 0};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.extent = {1, 1, 1};
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.mipLevelCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.layerCount = 1;
		scene.staticGeometry.lightmapAtlasImageAssetDescriptor.data = {0, 0, 0, 1};
		scene.dynamicGeometry.materialShaders = {};
		scene.dynamicGeometry.pointLights = {};
		scene.dynamicGeometry.entityCount = 0;
		scene.dynamicGeometry.geometryMappings = {};
		scene.dynamicGeometry.rigidBodyMappings = {};

		return scene;
	}
}