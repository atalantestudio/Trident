// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/RigidBody/RigidBody2D.hpp"
#include "AssetBuilder/API/RigidBody/RigidBody3D.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/API/Geometry/Index.hpp"
#include "Engine/API/Graphics/VertexInputAttributeFormat.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"

namespace Trident {
	BuildGeometry3D decodeFBXGeometry(view<uint8> inputData, uint32 identifier);

	/// Uses the FBX geometry streams to create a 3D geometry containing a vertex and an index stream.
	/// The index streams must have the same size.
	/// The resulting geometry has no hull.
	BuildGeometry3D inflateFBXStreams(
		view<svec3<float32>> positions,
		view<svec3<float32>> normals,
		view<svec2<float32>> textureCoordinates,
		view<uint32> positionIndices,
		view<uint32> normalIndices,
		view<uint32> textureCoordinateIndices
	);

	/// Generates the vertex tangents of a geometry.
	/// See https://marti.works/posts/post-calculating-tangents-for-your-mesh/post.
	void generateTangents(BuildGeometry3D& geometry);

	template<Hull2DType HullType>
	Hull2D createHull(const Geometry2D& geometry, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer);

	template<Hull3DType HullType>
	Hull3D createHull(view<BuildVertex3D> vertices);

	RigidBody2DComponent createRigidBody(const Descriptor<RigidBody2DComponent>& descriptor);
	RigidBody3DComponent createRigidBody(const Descriptor<RigidBody3DComponent>& descriptor);

	ImageAssetDescriptor openImage(view<char8> path);

	template<VertexInputAttributeFormat Format, typename Output, typename Input>
	Output pack(Input value);

	template<VertexInputAttributeFormat Format, typename Output, typename Input>
	Output unpack(Input value);
}