// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeFormat.hpp"

namespace Trident::DX12 {
	AccelerationStructureBuildSizes getAccelerationStructureBuildSizesImplementation(const Trident::Device* device, const AccelerationStructureBuildGeometryDescriptor& descriptor) {
		const DX12::Device* Device = static_cast<const DX12::Device*>(device);

		sequence<D3D12_RAYTRACING_GEOMETRY_DESC> GeometryDescs(descriptor.geometries.count());
		UINT GeometryIndex = 0;

		while (GeometryIndex < GeometryDescs.count()) {
			const AccelerationStructureTriangleGeometryDescriptor& geometry = descriptor.geometries[GeometryIndex];

			D3D12_RAYTRACING_GEOMETRY_DESC& GeometryDesc = GeometryDescs[GeometryIndex];
			GeometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			GeometryDesc.Flags = translateGeometryFlags(geometry.flags);
			GeometryDesc.Triangles.Transform3x4 = 0;
			GeometryDesc.Triangles.IndexFormat = getIndexBufferFormat(geometry.index.byteSize);
			GeometryDesc.Triangles.VertexFormat = getVertexInputAttributeFormat(geometry.vertex.format);
			GeometryDesc.Triangles.IndexCount = geometry.index.count;
			GeometryDesc.Triangles.VertexCount = geometry.vertex.count;
			GeometryDesc.Triangles.IndexBuffer = ref_cast<DX12::Buffer>(geometry.index.buffer)->resource->GetGPUVirtualAddress();
			GeometryDesc.Triangles.VertexBuffer.StartAddress = ref_cast<DX12::Buffer>(geometry.vertex.buffer)->resource->GetGPUVirtualAddress();
			GeometryDesc.Triangles.VertexBuffer.StrideInBytes = geometry.vertex.stride;

			GeometryIndex += 1;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS BuildInputs{};
		BuildInputs.Type = translateAccelerationStructureType(descriptor.type);
		BuildInputs.Flags = translateAccelerationStructureBuildFlags(descriptor.flags);
		BuildInputs.NumDescs = static_cast<uint32>(GeometryDescs.count());
		BuildInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		// TODO(graphics/dx12/ext_hardware_ray_tracing/geometry-pointers): Expose both pGeometryDescs and ppGeometryDescs?
		BuildInputs.pGeometryDescs = &GeometryDescs[0];

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO PrebuildInfo;

		Device->device5->GetRaytracingAccelerationStructurePrebuildInfo(&BuildInputs, &PrebuildInfo);

		AccelerationStructureBuildSizes buildSizes{};
		buildSizes.accelerationStructureSize = PrebuildInfo.ResultDataMaxSizeInBytes;
		buildSizes.scratchBuildSize = PrebuildInfo.ScratchDataSizeInBytes;
		buildSizes.scratchUpdateSize = PrebuildInfo.UpdateScratchDataSizeInBytes;

		return buildSizes;
	}
}