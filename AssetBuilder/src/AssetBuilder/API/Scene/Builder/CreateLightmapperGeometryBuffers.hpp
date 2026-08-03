// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<GeometryIndex3D>& vertices, sequence<svec3<float32>>& positions, sequence<svec3<float32>>& normals) {
		vertices = sequence<GeometryIndex3D>(staticVertexCount);
		positions = sequence<svec3<float32>>(staticPositionCount);
		normals = sequence<svec3<float32>>(staticNormalCount);

		std::copy(scene.staticGeometry.bakedVertices.begin(), scene.staticGeometry.bakedVertices.end(), &vertices[0]);
		std::copy(scene.staticGeometry.bakedPositions.begin(), scene.staticGeometry.bakedPositions.end(), &positions[0]);
		std::copy(scene.staticGeometry.bakedNormals.begin(), scene.staticGeometry.bakedNormals.end(), &normals[0]);

		uint32 vertexOffset = static_cast<uint32>(scene.staticGeometry.bakedVertices.count());
		uint32 positionOffset = static_cast<uint32>(scene.staticGeometry.bakedPositions.count());
		uint32 normalOffset = static_cast<uint32>(scene.staticGeometry.bakedNormals.count());

		for (const IntermediateMaterialShader& materialShader : staticMaterialShaders) {
			for (const IntermediateMaterial& material : materialShader.materials) {
				for (const IntermediateMaterialGeometry& materialGeometry : material.geometries) {
					const IntermediateGeometry& geometry = geometries[materialGeometry.geometryIndex];

					if (geometry.assetIdentifier == 0) {
						continue;
					}

					for (const BuildTransform3D& instance : materialGeometry.instances) {
						const quat<float32> orientation = normalized(
							quat<float32>::axisAngle(UP, instance.rotation[1]) *
							quat<float32>::axisAngle(RIGHT, instance.rotation[0]) *
							quat<float32>::axisAngle(FORWARD, instance.rotation[2])
						);
						const quat<float32> inverseOrientation = inverse(orientation);
						const mat4x4<float32> world = (
							mat4x4<float32>::scaling({instance.scale, instance.scale, instance.scale}) *
							mat4x4<float32>(orientation) *
							mat4x4<float32>::translation(instance.translation)
						);

						uint32 vertexIndex = 0;

						while (vertexIndex < geometry.vertices.count()) {
							const GeometryIndex3D& vertex = geometry.vertices[vertexIndex];

							vertices[vertexOffset + vertexIndex] = {
								.positionIndex = positionOffset + vertex.positionIndex,
								.normalIndex = normalOffset + vertex.normalIndex,
								.textureCoordinateIndex = 0,
							};

							vertexIndex += 1;
						}

						uint32 positionIndex = 0;

						while (positionIndex < geometry.positions.count()) {
							// TODO(lyah/scalar)
							positions[positionOffset + positionIndex] = svec3<float32>(multiplyTransform(vec3<float32>(geometry.positions[positionIndex]), world));

							positionIndex += 1;
						}

						uint32 normalIndex = 0;

						while (normalIndex < geometry.normals.count()) {
							// TODO(lyah/scalar)
							normals[normalOffset + normalIndex] = svec3<float32>(normalized(vec3<float32>(geometry.normals[normalIndex]) * inverseOrientation));

							normalIndex += 1;
						}

						vertexOffset += vertexIndex;
						positionOffset += positionIndex;
						normalOffset += normalIndex;
					}
				}
			}
		}
	}
}