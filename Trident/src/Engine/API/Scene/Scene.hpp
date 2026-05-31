// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/ECS/ECS.hpp"

namespace Trident {
	class Scene;

	template<>
	struct Descriptor<Scene> {
		uint64 dynamicVertexCount;
		uint64 dynamicIndexCount;
	};

	class Scene {
		public:
			explicit Scene(const Descriptor<Scene>& descriptor) :
				dynamicVertexCount(descriptor.dynamicVertexCount),
				dynamicIndexCount(descriptor.dynamicIndexCount)
			{}

			inline uint64 getDynamicVertexCount() const {
				return dynamicVertexCount;
			}

			inline uint64 getDynamicIndexCount() const {
				return dynamicIndexCount;
			}

			inline const sequence<uint8>& getDataBuffer() const {
				return dataBuffer;
			}

		public:
			ECS ecs;

		protected:
			uint64 dynamicVertexCount;
			uint64 dynamicIndexCount;

			// NOTE: Buffer layout:
			// - Vertex position components of each geometry, in order
			// - Vertex normal components of each geometry, in order
			// - Vertex texture coordinate components of each geometry, in order
			// - Vertex position indices of each geometry, in order (TRIANGLE_LIST)
			// - Vertex position indices of each hull, in order (LINE_STRIP for 2D, LINE_LIST for 3D)
			// - Vertex normal indices of each geometry, in order
			// - Vertex texture coordinate indices of each geometry, in order
			/// Contains dynamic (external geometries) vertices and indices.
			sequence<uint8> dataBuffer;
	};
}