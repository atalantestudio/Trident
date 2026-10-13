// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::createRigidBody(const EntityDescriptor& descriptor) {
		const uint32 materialShaderIndex = descriptor.material >> 16;
		const uint32 materialIndex = descriptor.material & 0xff;

		IntermediateMaterialShader& materialShader = materialShaders[materialShaderIndex];

		ENGINE_ASSERT(materialShader.asset.geometryType == GeometryType::DYNAMIC, "Cannot create dynamic geometry under a static material.");

		IntermediateMaterial& material = materialShader.materials[materialIndex];

		const uint32 materialGeometryIndex = findMaterialGeometryIndex(material, descriptor.geometryIndex);

		if (materialGeometryIndex >= material.geometries.size()) {
			const IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];

			IntermediateMaterialGeometry materialGeometry{};
			materialGeometry.geometryIndex = descriptor.geometryIndex;

			material.geometries.emplace_back(materialGeometry);

			dynamicVertexCount += geometry.vertices.count();
			dynamicIndexCount += geometry.indices.count();
			materialGeometryCount += 1;
		}

		IntermediateGeometry& geometry = geometries[descriptor.geometryIndex];
		geometry.sharedInstanceCount += 1;

		IntermediateMaterialGeometry& materialGeometry = material.geometries[materialGeometryIndex];

		EntityComponentMapping<Geometry3DComponent> geometryMapping{};
		geometryMapping.entity = entityCount;
		geometryMapping.component.geometryIndex = descriptor.geometryIndex;

		geometryMappings.emplace_back(geometryMapping);

		materialGeometry.instances.emplace_back();

		EntityComponentMapping<RigidBody3DComponent> rigidBodyMapping{};
		rigidBodyMapping.entity = entityCount;
		rigidBodyMapping.component = Trident::createRigidBody({
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