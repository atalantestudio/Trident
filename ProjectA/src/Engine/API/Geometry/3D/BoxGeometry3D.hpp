#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"

namespace ProjectA {
	class BoxGeometry3D : public Geometry3D {
		private:
			static inline const sequence<vec3<float32>> VERTEX_POSITIONS = {
				{ 0.5f, -0.5f,  0.5f},
				{ 0.5f,  0.5f,  0.5f},
				{-0.5f,  0.5f,  0.5f},
				{-0.5f, -0.5f,  0.5f},
				{-0.5f, -0.5f, -0.5f},
				{-0.5f,  0.5f, -0.5f},
				{ 0.5f,  0.5f, -0.5f},
				{ 0.5f, -0.5f, -0.5f},
			};

			static inline const sequence<uint32> TRIANGLE_VERTEX_POSITION_INDICES = {
				// Front (+Z)
				0, 1, 2,
				0, 2, 3,
				// Back (-Z)
				4, 5, 6,
				4, 6, 7,
				// Left (-X)
				3, 2, 5,
				3, 5, 4,
				// Right (+X)
				7, 6, 1,
				7, 1, 0,
				// Top (+Y)
				5, 2, 1,
				5, 1, 6,
				// Bottom (-Y)
				3, 4, 7,
				3, 7, 0,
			};

			static inline const sequence<vec3<float32>> VERTEX_NORMALS = {
				{ 0.0f,  0.0f,  1.0f},
				{ 0.0f,  0.0f, -1.0f},
				{-1.0f,  0.0f,  0.0f},
				{ 1.0f,  0.0f,  0.0f},
				{ 0.0f,  1.0f,  0.0f},
				{ 0.0f, -1.0f,  0.0f},
			};

			static inline const sequence<uint32> VERTEX_NORMAL_INDICES = {
				// Front (+Z)
				0, 0, 0,
				0, 0, 0,
				// Back (-Z)
				1, 1, 1,
				1, 1, 1,
				// Left (-X)
				2, 2, 2,
				2, 2, 2,
				// Right (+X)
				3, 3, 3,
				3, 3, 3,
				// Top (+Y)
				4, 4, 4,
				4, 4, 4,
				// Bottom (-Y)
				5, 5, 5,
				5, 5, 5,
			};

			static inline const sequence<vec2<float32>> VERTEX_TEXTURE_COORDINATES = {
				{0, 0},
				{1, 0},
				{0, 1},
				{1, 1},
			};

			static inline const sequence<uint32> VERTEX_TEXTURE_COORDINATE_INDICES = {
				// Front (+Z)
				0, 2, 3,
				0, 3, 1,
				// Back (-Z)
				0, 2, 3,
				0, 3, 1,
				// Left (-X)
				0, 2, 3,
				0, 3, 1,
				// Right (+X)
				0, 2, 3,
				0, 3, 1,
				// Top (+Y)
				0, 2, 3,
				0, 3, 1,
				// Bottom (-Y)
				0, 2, 3,
				0, 3, 1,
			};

		public:
			explicit BoxGeometry3D(const vec3<float32>& size) {
				const vec3<float32> halfSize = size * 0.5f;

				vertexPositions = VERTEX_POSITIONS;

				for (uint32 vertexIndex = 0; vertexIndex < VERTEX_POSITIONS.count; vertexIndex += 1) {
					vertexPositions[vertexIndex] *= halfSize;
				}

				vertexPositionIndices = TRIANGLE_VERTEX_POSITION_INDICES;
				vertexNormals = VERTEX_NORMALS;
				vertexNormalIndices = VERTEX_NORMAL_INDICES;
				vertexTextureCoordinates = VERTEX_TEXTURE_COORDINATES;
				vertexTextureCoordinateIndices = VERTEX_TEXTURE_COORDINATE_INDICES;
			}
	};
}