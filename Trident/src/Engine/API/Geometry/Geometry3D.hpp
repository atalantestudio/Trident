// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/API/Geometry/Geometry.hpp"

namespace Trident {
	class Geometry3D;

	template<>
	struct Descriptor<Geometry3D> {
		uint32 triangleCount;
		range<uint32, uint32> vertexPositionComponentRange;
		range<uint32, uint32> vertexNormalComponentRange;
		range<uint32, uint32> vertexTextureCoordinateComponentRange;
		uint32 triangleTangentComponentOffset;

		/// Indices for the default visualization mode (TRIANGLE_LIST).
		uint32 vertexPositionIndexOffset;
		uint32 vertexNormalIndexOffset;
		uint32 vertexTextureCoordinateIndexOffset;
	};

	class Geometry3D : public Geometry {
		public:
			explicit Geometry3D() = default;

			explicit Geometry3D(const Descriptor<Geometry3D>& descriptor) :
				indexCount(descriptor.triangleCount * 3),
				vertexPositionComponentRange(descriptor.vertexPositionComponentRange),
				vertexNormalComponentRange(descriptor.vertexNormalComponentRange),
				vertexTextureCoordinateComponentRange(descriptor.vertexTextureCoordinateComponentRange),
				triangleTangentComponentOffset(descriptor.triangleTangentComponentOffset),
				vertexPositionIndexOffset(descriptor.vertexPositionIndexOffset),
				vertexNormalIndexOffset(descriptor.vertexNormalIndexOffset),
				vertexTextureCoordinateIndexOffset(descriptor.vertexTextureCoordinateIndexOffset)
			{}

			inline range<uint32, uint32> getVertexPositionComponentRange() const {
				return vertexPositionComponentRange;
			}

			inline range<uint32, uint32> getVertexNormalComponentRange() const {
				return vertexNormalComponentRange;
			}

			inline range<uint32, uint32> getVertexTextureCoordinateComponentRange() const {
				return vertexTextureCoordinateComponentRange;
			}

			inline range<uint32, uint32> getTriangleTangentComponentRange() const {
				return {triangleTangentComponentOffset, indexCount};
			}

			inline range<uint32, uint32> getVertexPositionIndexRange() const {
				return {vertexPositionIndexOffset, indexCount};
			}

			inline range<uint32, uint32> getVertexNormalIndexRange() const {
				return {vertexNormalIndexOffset, indexCount};
			}

			inline range<uint32, uint32> getVertexTextureCoordinateIndexRange() const {
				return {vertexTextureCoordinateIndexOffset, indexCount};
			}

		protected:
			uint32 indexCount;
			range<uint32, uint32> vertexPositionComponentRange;
			range<uint32, uint32> vertexNormalComponentRange;
			range<uint32, uint32> vertexTextureCoordinateComponentRange;
			uint32 triangleTangentComponentOffset;

			/// Indices for the default visualization mode (TRIANGLE_LIST).
			uint32 vertexPositionIndexOffset;
			uint32 vertexNormalIndexOffset;
			uint32 vertexTextureCoordinateIndexOffset;
	};
}