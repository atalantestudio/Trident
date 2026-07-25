// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/MeshSceneBuilder.hpp"

namespace Trident {
	void MeshSceneBuilder::placeMesh(const StaticGeometryDescriptor& descriptor) {
		const view<float32> vertexComponents = descriptor.asset->getVertexPositionComponents();
		const view<float32> normalComponents = descriptor.asset->getVertexNormalComponents();
		const view<float32> textureCoordinateComponents = descriptor.asset->getVertexTextureCoordinateComponents();
		const view<uint32> vertexIndices = descriptor.asset->getVertexPositionIndices();
		const view<uint32> normalIndices = descriptor.asset->getVertexNormalIndices();
		const view<uint32> textureCoordinateIndices = descriptor.asset->getVertexTextureCoordinateIndices();
		const uint64 vertexCount = vertexComponents.count() / 3;
		const uint64 normalCount = normalComponents.count() / 3;
		const uint64 textureCoordinateCount = textureCoordinateComponents.count() / 2;
		const uint32 triangleCount = static_cast<uint32>(vertexIndices.count() / 3);

		const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, descriptor.transform.eulerAngles[0]);
		const quat<float32> yaw = quat<float32>::axisAngle(UP, descriptor.transform.eulerAngles[1]);
		const quat<float32> roll = quat<float32>::axisAngle(FORWARD, descriptor.transform.eulerAngles[2]);
		const quat<float32> orientation = normalized(yaw * pitch * roll);
		const mat4x4<float32> worldTransform = (
			mat4x4<float32>::scaling(descriptor.transform.scale) *
			mat4x4<float32>(orientation) *
			mat4x4<float32>::translation(descriptor.transform.translation)
		);

		const uint32 vertexOffset = static_cast<uint32>(vertices.size());
		const uint32 normalOffset = static_cast<uint32>(normals.size());
		const uint32 textureCoordinateOffset = static_cast<uint32>(textureCoordinates.size());

		BuildStaticMaterial3D& material = materials[descriptor.materialIndex];
		std::vector<StaticTriangle3D>& triangles = trianglesPerMaterial[descriptor.materialIndex];

		svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
		svec2<float32> minCoordinate = {max<float32>(), max<float32>()};

		StaticSubsurface subsurface{};
		subsurface.vertices = sequence<svec3<float32>>(vertexCount);
		subsurface.textureCoordinates = sequence<svec2<float32>>(textureCoordinateCount);
		subsurface.vertexIndices = descriptor.asset->getVertexPositionIndices();
		subsurface.textureCoordinateIndices = descriptor.asset->getVertexTextureCoordinateIndices();

		uint32 vertexIndex = 0;

		while (vertexIndex < vertexCount) {
			// TODO(lyah/scalar)
			const svec3<float32> vertex = svec3<float32>(multiplyTransform({
				vertexComponents[vertexIndex * 3 + 0],
				vertexComponents[vertexIndex * 3 + 1],
				vertexComponents[vertexIndex * 3 + 2],
			}, worldTransform));

			subsurface.vertices[vertexIndex] = vertex;

			vertices.emplace_back(vertex);

			vertexIndex += 1;
		}

		uint32 textureCoordinateIndex = 0;

		while (textureCoordinateIndex < textureCoordinateCount) {
			// TODO(lyah/scalar)
			const svec2<float32> textureCoordinate = {
				textureCoordinateComponents[textureCoordinateIndex * 2 + 0],
				textureCoordinateComponents[textureCoordinateIndex * 2 + 1],
			};

			subsurface.textureCoordinates[textureCoordinateIndex] = textureCoordinate;

			textureCoordinates.emplace_back(textureCoordinate);

			maxCoordinate.x = max(maxCoordinate.x, textureCoordinate.x);
			maxCoordinate.y = max(maxCoordinate.y, textureCoordinate.y);
			minCoordinate.x = min(minCoordinate.x, textureCoordinate.x);
			minCoordinate.y = min(minCoordinate.y, textureCoordinate.y);

			textureCoordinateIndex += 1;
		}

		// TODO(asset-builder/scene/mesh): Better scale calculation.
		subsurface.scale = (maxCoordinate - minCoordinate) * 968.0f;

		StaticSurface surface{};
		surface.vertices = {};
		surface.textureCoordinates = {};
		surface.edgeOriginVertexIndices = {};
		surface.normal = {0.0f, 1.0f, 0.0f};
		surface.scale = subsurface.scale;
		surface.subsurfaces = {subsurface};
		surface.textureFiltering = {};
		surface.triangleOffset = static_cast<uint32>(triangles.size());
		surface.materialIndex = descriptor.materialIndex;
		surface.luxelScale = descriptor.luxelScale;
		surface.minCoordinate = minCoordinate;

		uint32 triangleIndex = 0;
		uint32 indexOffset = 0;

		while (triangleIndex < triangleCount) {
			const uint32 vertexIndexA = subsurface.vertexIndices[indexOffset + 0];
			const uint32 vertexIndexB = subsurface.vertexIndices[indexOffset + 1];
			const uint32 vertexIndexC = subsurface.vertexIndices[indexOffset + 2];

			const vec3<float32> a = subsurface.vertices[vertexIndexA];
			const vec3<float32> b = subsurface.vertices[vertexIndexB];
			const vec3<float32> c = subsurface.vertices[vertexIndexC];

			const vec3<float32> normal = normalized(cross(b - a, c - a));

			normals.emplace_back(normal);

			const uint32 textureCoordinateIndexA = subsurface.textureCoordinateIndices[indexOffset + 0];
			const uint32 textureCoordinateIndexB = subsurface.textureCoordinateIndices[indexOffset + 1];
			const uint32 textureCoordinateIndexC = subsurface.textureCoordinateIndices[indexOffset + 2];

			const StaticTriangle3D triangle = {
				.vertices = {
					{
						.vertexIndex = vertexOffset + vertexIndexA,
						.textureCoordinateIndex = textureCoordinateOffset + textureCoordinateIndexA,
						.lightmapCoordinate = {0.0f, 0.0f},
					},
					{
						.vertexIndex = vertexOffset + vertexIndexB,
						.textureCoordinateIndex = textureCoordinateOffset + textureCoordinateIndexB,
						.lightmapCoordinate = {0.0f, 0.0f},
					},
					{
						.vertexIndex = vertexOffset + vertexIndexC,
						.textureCoordinateIndex = textureCoordinateOffset + textureCoordinateIndexC,
						.lightmapCoordinate = {0.0f, 0.0f},
					},
				},
				.normalIndex = normalOffset + triangleIndex,
			};

			triangles.emplace_back(triangle);

			triangleIndex += 1;
			indexOffset += 3;
		}

		surfaces.emplace_back(surface);

		material.triangleRange.count += triangleCount;
		this->triangleCount += triangleCount;
		subsurfaceTextureCoordinateCount += static_cast<uint32>(textureCoordinateCount);
		subsurfaceCount += 1;
	}
}