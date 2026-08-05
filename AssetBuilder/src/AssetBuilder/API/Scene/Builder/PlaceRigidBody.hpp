// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/SceneBuilder.hpp"
#include "../../../../Subprocess/RigidBody.hpp"

namespace Trident {
	void SceneBuilder::placeRigidBody(const EntityDescriptor& descriptor) {
		const uint32 materialShaderIndex = descriptor.material >> 16;
		const uint32 materialIndex = descriptor.material & 0xff;

		IntermediateMaterial& material = dynamicMaterialShaders[materialShaderIndex].materials[materialIndex];

		const uint32 materialGeometryIndex = findMaterialGeometryIndex(material, descriptor.geometryIndex);

		if (materialGeometryIndex >= material.geometries.size()) {
			const IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = descriptor.geometryIndex;

			material.geometries.emplace_back(materialGeometry);

			dynamicVertexCount += geometry.vertices.count();
			dynamicPositionCount += geometry.positions.count();
			dynamicNormalCount += geometry.normals.count();
			dynamicTextureCoordinateCount += geometry.textureCoordinates.count();
			materialGeometryCount += 1;
		}

		IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
		geometry.sharedInstanceCount += 1;

		IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];

		EntityComponentMapping<GeometryComponent> geometryMapping{};
		geometryMapping.entity = entityCount;
		geometryMapping.component.geometryIndex = descriptor.geometryIndex;

		geometryMappings.emplace_back(geometryMapping);

		materialGeometry.instances.emplace_back();

		EntityComponentMapping<RigidBody3DComponent> rigidBodyMapping{};
		rigidBodyMapping.entity = entityCount;
		rigidBodyMapping.component = createRigidBody({
			.position = descriptor.position,
			.rotation = descriptor.rotation,
			.uniformScale = descriptor.uniformScale,
			.linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s},
			.angularVelocity = {0.0_w, 0.0_w, 0.0_w},
			.mass = descriptor.mass,
			.momentOfInertia = descriptor.momentOfInertia,
			.inertiaScale = descriptor.inertiaScale,
			.friction = descriptor.friction,
			.restitution = descriptor.restitution,
		});

		rigidBodyMappings.emplace_back(rigidBodyMapping);

		entityCount += 1;
	}
}