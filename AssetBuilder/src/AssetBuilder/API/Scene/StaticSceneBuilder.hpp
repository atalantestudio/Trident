// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticObject.hpp"

namespace Trident {
	class StaticSceneBuilder {
		public:
			explicit StaticSceneBuilder();

			view<svec3<float32>> getVertexBuffer() const;

			view<uint16> getIndexBuffer() const;

			view<StaticObject> getObjects() const;

			void createStaticObject(const StaticObjectDescriptor& descriptor);

			uint16 findVertex(svec3<float32> vertex) const;

		private:
			std::vector<svec3<float32>> vertexBuffer;
			std::vector<uint16> indexBuffer;
			std::vector<StaticObject> objects;
	};
}