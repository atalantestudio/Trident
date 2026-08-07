// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::createLightmapperGeometryBuffers(const BuildScene3D& scene, sequence<BuildVertex3D>& vertices, sequence<uint32>& indices) {
		vertices = sequence<BuildVertex3D>(staticVertexCount);
		indices = sequence<uint32>(staticIndexCount);

		std::copy(scene.staticGeometry.bakedVertices.begin(), scene.staticGeometry.bakedVertices.end(), &vertices[0]);
		std::copy(scene.staticGeometry.bakedIndices.begin(), scene.staticGeometry.bakedIndices.end(), &indices[0]);

		uint32 vertexOffset = static_cast<uint32>(bakedVertexCount);
		uint32 indexOffset = static_cast<uint32>(bakedIndexCount);

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
						const mat4x4<float32> worldTransform = (
							mat4x4<float32>::scaling({instance.scale, instance.scale, instance.scale}) *
							mat4x4<float32>(orientation) *
							mat4x4<float32>::translation(instance.translation)
						);

						uint32 vertexIndex = 0;

						while (vertexIndex < geometry.vertices.count()) {
							const BuildVertex3D& geometryVertex = geometry.vertices[vertexIndex];

							BuildVertex3D& vertex = vertices[vertexOffset + vertexIndex];

							// TODO(lyah/scalar)
							vertex.position = svec3<float32>(multiplyTransform(vec3<float32>(geometryVertex.position), worldTransform));
							vertex.normal = svec3<float32>(normalized(vec3<float32>(geometryVertex.normal) * inverseOrientation));
							vertex.tangent = svec3<float32>(normalized(vec3<float32>(geometryVertex.tangent) * inverseOrientation));

							vertex.textureCoordinate = geometryVertex.textureCoordinate;

							vertexIndex += 1;
						}

						uint32 indexIndex = 0;

						while (indexIndex < geometry.indices.count()) {
							indices[indexOffset + indexIndex] = vertexOffset + geometry.indices[indexIndex];

							indexIndex += 1;
						}

						vertexOffset += vertexIndex;
						indexOffset += indexIndex;
					}
				}
			}
		}
	}
}