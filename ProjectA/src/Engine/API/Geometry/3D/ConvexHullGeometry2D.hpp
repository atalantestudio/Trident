#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"

namespace ProjectA {
	struct ConvexHullGeometry3DDescriptor {
		sequence<vec3<float32>> vertexPositions;
		sequence<uint32> vertexPositionIndices;
		sequence<vec3<float32>> vertexNormals;
		sequence<uint32> vertexNormalIndices;
		sequence<vec2<float32>> vertexTextureCoordinates;
		sequence<uint32> vertexTextureCoordinateIndices;
	};

	class ConvexHullGeometry3D : public Geometry3D {
		public:
			explicit ConvexHullGeometry3D(const ConvexHullGeometry3DDescriptor& descriptor) {
				vertexPositions = descriptor.vertexPositions;
				vertexPositionIndices = descriptor.vertexPositionIndices;
				vertexNormals = descriptor.vertexNormals;
				vertexNormalIndices = descriptor.vertexNormalIndices;
				vertexTextureCoordinates = descriptor.vertexTextureCoordinates;
				vertexTextureCoordinateIndices = descriptor.vertexTextureCoordinateIndices;
			}
	};
}