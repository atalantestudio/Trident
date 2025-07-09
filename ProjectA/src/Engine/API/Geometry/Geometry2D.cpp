#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	Geometry2D::Geometry2D(const Geometry2DDescriptor& descriptor) :
		indices(descriptor.indices),
		vertices(descriptor.vertices)
	{
		DEBUG_ASSERT(indices.count > 0 && vertices.count > 0);
	}

	uint32 Geometry2D::getIndexCount() const {
		return static_cast<uint32>(indices.count);
	}

	uint32 Geometry2D::getVertexCount() const {
		return static_cast<uint32>(vertices.count);
	}
}