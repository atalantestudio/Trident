// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	void Scene3D::createVertexBuffer(uint64 minStorageBufferOffsetAlignment) {
		vertexPositionComponentBufferSize = 0;
		vertexPositionIndexBufferSize = 0;
		vertexNormalComponentBufferSize = 0;
		vertexNormalIndexBufferSize = 0;
		vertexTextureCoordinateComponentBufferSize = 0;
		vertexTextureCoordinateIndexBufferSize = 0;
		vertexTangentComponentBufferSize = 0;

		uint32 geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			const ref<Geometry3D> geometry = geometries[geometryIndex];

			vertexPositionComponentBufferSize += geometry->getVertexPositionComponents().count();
			vertexPositionIndexBufferSize += geometry->getVertexPositionIndices().count();
			vertexNormalComponentBufferSize += geometry->getVertexNormalComponents().count();
			vertexNormalIndexBufferSize += geometry->getVertexNormalIndices().count();
			vertexTextureCoordinateComponentBufferSize += geometry->getVertexTextureCoordinateComponents().count();
			vertexTextureCoordinateIndexBufferSize += geometry->getVertexTextureCoordinateIndices().count();
			vertexTangentComponentBufferSize += geometry->getVertexTangentComponents().count();

			geometryIndex += 1;
		}

		vertexPositionComponentBufferSize = ceil(vertexPositionComponentBufferSize * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexPositionIndexBufferSize = ceil(vertexPositionIndexBufferSize * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexNormalComponentBufferSize = ceil(vertexNormalComponentBufferSize * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexNormalIndexBufferSize = ceil(vertexNormalIndexBufferSize * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateComponentBufferSize = ceil(vertexTextureCoordinateComponentBufferSize * sizeof(float32), minStorageBufferOffsetAlignment);
		vertexTextureCoordinateIndexBufferSize = ceil(vertexTextureCoordinateIndexBufferSize * sizeof(uint32), minStorageBufferOffsetAlignment);
		vertexTangentComponentBufferSize = ceil(vertexTangentComponentBufferSize * sizeof(float32), minStorageBufferOffsetAlignment);

		vertexBuffer = sequence<uint8>(vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize + vertexTextureCoordinateIndexBufferSize + vertexTangentComponentBufferSize);

		uint8* range = &vertexBuffer[0];
		float32* vertexPositionComponentRange = pointer_cast<float32*>(range);
		uint32* vertexPositionIndexRange = pointer_cast<uint32*>(range + vertexPositionComponentBufferSize);
		float32* vertexNormalComponentRange = pointer_cast<float32*>(range + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize);
		uint32* vertexNormalIndexRange = pointer_cast<uint32*>(range + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize);
		float32* vertexTextureCoordinateComponentRange = pointer_cast<float32*>(range + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize);
		uint32* vertexTextureCoordinateIndexRange = pointer_cast<uint32*>(range + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize);
		float32* vertexTangentComponentRange = pointer_cast<float32*>(range + vertexPositionComponentBufferSize + vertexPositionIndexBufferSize + vertexNormalComponentBufferSize + vertexNormalIndexBufferSize + vertexTextureCoordinateComponentBufferSize + vertexTextureCoordinateIndexBufferSize);

		geometryIndex = 0;

		while (geometryIndex < geometryCount) {
			const ref<Geometry3D> geometry = geometries[geometryIndex];

			std::copy_n(&geometry->getVertexPositionComponents()[0], geometry->getVertexPositionComponents().count(), vertexPositionComponentRange);
			std::copy_n(&geometry->getVertexPositionIndices()[0], geometry->getVertexPositionIndices().count(), vertexPositionIndexRange);
			std::copy_n(&geometry->getVertexNormalComponents()[0], geometry->getVertexNormalComponents().count(), vertexNormalComponentRange);
			std::copy_n(&geometry->getVertexNormalIndices()[0], geometry->getVertexNormalIndices().count(), vertexNormalIndexRange);
			std::copy_n(&geometry->getVertexTextureCoordinateComponents()[0], geometry->getVertexTextureCoordinateComponents().count(), vertexTextureCoordinateComponentRange);
			std::copy_n(&geometry->getVertexTextureCoordinateIndices()[0], geometry->getVertexTextureCoordinateIndices().count(), vertexTextureCoordinateIndexRange);
			std::copy_n(&geometry->getVertexTangentComponents()[0], geometry->getVertexTangentComponents().count(), vertexTangentComponentRange);

			vertexPositionComponentRange += geometry->getVertexPositionComponents().count();
			vertexPositionIndexRange += geometry->getVertexPositionIndices().count();
			vertexNormalComponentRange += geometry->getVertexNormalComponents().count();
			vertexNormalIndexRange += geometry->getVertexNormalIndices().count();
			vertexTextureCoordinateComponentRange += geometry->getVertexTextureCoordinateComponents().count();
			vertexTextureCoordinateIndexRange += geometry->getVertexTextureCoordinateIndices().count();
			vertexTangentComponentRange += geometry->getVertexTangentComponents().count();
			geometryIndex += 1;
		}
	}
}