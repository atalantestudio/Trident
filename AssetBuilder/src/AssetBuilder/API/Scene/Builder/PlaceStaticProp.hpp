// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"

namespace Trident {
	void SceneBuilder::placeStaticProp(const MeshDescriptor& descriptor) {
		IntermediateMaterial& material = getMaterial(descriptor.material);

		const uint32 materialGeometryIndex = findMaterialGeometryIndex(material, descriptor.geometryIndex);

		if (materialGeometryIndex >= material.geometries.size()) {
			const IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = descriptor.geometryIndex;

			material.geometries.emplace_back(materialGeometry);

			staticVertexCount += geometry.vertices.count();
			staticPositionCount += geometry.positions.count();
			staticNormalCount += geometry.normals.count();
			staticTextureCoordinateCount += geometry.textureCoordinates.count();
			materialGeometryCount += 1;
		}

		IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
		geometry.sharedInstanceCount += 1;

		IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];

		const quat<float32> orientation = normalized(
			quat<float32>::axisAngle(UP, descriptor.transform.rotation[1]) *
			quat<float32>::axisAngle(RIGHT, descriptor.transform.rotation[0]) *
			quat<float32>::axisAngle(FORWARD, descriptor.transform.rotation[2])
		);
		const quat<float32> inverseOrientation = inverse(orientation);
		const mat4x4<float32> world = (
			mat4x4<float32>::scaling({descriptor.transform.scale, descriptor.transform.scale, descriptor.transform.scale}) *
			mat4x4<float32>(orientation) *
			mat4x4<float32>::translation(descriptor.transform.translation)
		);

		svec2<float32> maxCoordinate = {min<float32>(), min<float32>()};
		svec2<float32> minCoordinate = {max<float32>(), max<float32>()};

		uint32 textureCoordinateIndex = 0;

		while (textureCoordinateIndex < geometry.textureCoordinates.count()) {
			// TODO(lyah/scalar)
			const svec2<float32> textureCoordinate = geometry.textureCoordinates[textureCoordinateIndex];

			maxCoordinate.x = max(maxCoordinate.x, textureCoordinate.x);
			maxCoordinate.y = max(maxCoordinate.y, textureCoordinate.y);
			minCoordinate.x = min(minCoordinate.x, textureCoordinate.x);
			minCoordinate.y = min(minCoordinate.y, textureCoordinate.y);

			textureCoordinateIndex += 1;
		}

		const BuildTransform3D instance = {
			.translation = svec3<float32>(descriptor.transform.translation),
			.rotation = svec3<float32>(descriptor.transform.rotation),
			.scale = descriptor.transform.scale,
		};

		materialGeometry.instances.emplace_back(instance);

		Lightmap lightmap{};
		lightmap.geometryIndex = materialGeometry.geometryIndex;
		lightmap.vertexRange = {0, geometry.vertices.count()};
		lightmap.scale = (maxCoordinate - minCoordinate) * descriptor.lightmapScale;
		lightmap.minCoordinate = minCoordinate;
		lightmap.luxelScale = descriptor.lightmap.luxelScale;
		lightmap.tintColor = descriptor.lightmap.tintColor;
		lightmap.emissiveColor = descriptor.lightmap.emissiveColor;
		lightmap.emissiveIntensity = descriptor.lightmap.emissiveIntensity;

		lightmaps.emplace_back(lightmap);

		staticInstanceCount += 1;

		// TODO(asset-builder/scene/lightmap): Create less coordinates (1 per texture coordinate per instance).
		lightmapCoordinateCount += geometry.vertices.count();
	}
}