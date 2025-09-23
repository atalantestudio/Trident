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

			static inline const sequence<uint16> TRIANGLE_POSITION_INDICES = {
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

		public:
			explicit BoxGeometry3D(const vec3<float32>& size) : Geometry3D() {
				const vec3<float32> halfSize = size * 0.5f;

				vertices = VERTEX_POSITIONS;

				for (uint32 vertexIndex = 0; vertexIndex < VERTEX_POSITIONS.count; vertexIndex += 1) {
					vertices[vertexIndex] *= halfSize;
				}

				indices = TRIANGLE_POSITION_INDICES;
			}
	};
}