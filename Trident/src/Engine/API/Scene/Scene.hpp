// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Descriptor.hpp"
#include "Engine/ECS/ECS.hpp"

namespace Trident {
	class Scene;

	template<>
	struct Descriptor<Scene> {
		uint32 staticVertexCount;
		uint32 staticIndexCount;
	};

	class Scene {
		public:
			explicit Scene(const Descriptor<Scene>& descriptor) :
				staticVertexCount(descriptor.staticVertexCount),
				staticIndexCount(descriptor.staticIndexCount)
			{}

			inline uint32 getStaticVertexCount() const {
				return staticVertexCount;
			}

			inline uint32 getStaticIndexCount() const {
				return staticIndexCount;
			}

			inline uint32 getDynamicVertexCount() const {
				return dynamicVertexCount;
			}

			inline uint32 getDynamicIndexCount() const {
				return dynamicIndexCount;
			}

			inline const sequence<uint8>& getDataBuffer() const {
				return dataBuffer;
			}

		public:
			ECS ecs;

		protected:
			/// Contains static (internal to the scene) and dynamic (external geometries) vertices and indices.
			sequence<uint8> dataBuffer;

			uint32 staticVertexCount;
			uint32 staticIndexCount;
			uint32 dynamicVertexCount = 0;
			uint32 dynamicIndexCount = 0;
	};
}