// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/BLASTriangleGeometry.hpp"

namespace Trident::DX12 {
	void testCreateBLASTriangleGeometryImplementation(const Trident::Device* device, const BLASTriangleGeometryDescriptor& descriptor) {
		const ref<DX12::Buffer> vertexBuffer = ref_cast<DX12::Buffer>(descriptor.vertexBuffer);
		const ref<DX12::Buffer> indexBuffer = ref_cast<DX12::Buffer>(descriptor.indexBuffer);

		D3D12_RAYTRACING_GEOMETRY_DESC raytracingGeometryDescriptor{};
		raytracingGeometryDescriptor.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    	raytracingGeometryDescriptor.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE | D3D12_RAYTRACING_GEOMETRY_FLAG_NO_DUPLICATE_ANYHIT_INVOCATION;
		raytracingGeometryDescriptor.Triangles.Transform3x4 = 0;
		raytracingGeometryDescriptor.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
		raytracingGeometryDescriptor.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		// raytracingGeometryDescriptor.Triangles.IndexCount;
		// raytracingGeometryDescriptor.Triangles.VertexCount;
		raytracingGeometryDescriptor.Triangles.IndexBuffer = indexBuffer->resource->GetGPUVirtualAddress();
		raytracingGeometryDescriptor.Triangles.VertexBuffer.StartAddress = vertexBuffer->resource->GetGPUVirtualAddress();
		raytracingGeometryDescriptor.Triangles.VertexBuffer.StrideInBytes = sizeof(svec3<float32>);
	}
}