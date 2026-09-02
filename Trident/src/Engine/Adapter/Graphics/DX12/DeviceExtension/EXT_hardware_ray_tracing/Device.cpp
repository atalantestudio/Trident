// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildTriangleGeometry.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureType.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeFormat.hpp"

namespace Trident::DX12 {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor) {
		const DX12::Device* Device = static_cast<const DX12::Device*>(device);
		const BaseBufferView view = descriptor.view;

		ref<DX12::AccelerationStructure> accelerationStructure = allocateRef<DX12::AccelerationStructure>();
		accelerationStructure->Descriptor.DestAccelerationStructureData = pointer_cast<const DX12::Buffer*>(view.getBuffer())->resource->GetGPUVirtualAddress() + view.getOffset();
		accelerationStructure->Descriptor.Inputs = ref_cast<DX12::AccelerationStructureBuildTriangleGeometry>(descriptor.buildTriangleGeometry)->BuildInputs;
		accelerationStructure->Descriptor.SourceAccelerationStructureData = NULL;

		return accelerationStructure;
	}

	ref<Trident::AccelerationStructureBuildTriangleGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) {
		const DX12::Device* Device = static_cast<const DX12::Device*>(device);

		ref<DX12::AccelerationStructureBuildTriangleGeometry> accelerationStructureBuildTriangleGeometry = allocateRef<DX12::AccelerationStructureBuildTriangleGeometry>();

		sequence<D3D12_RAYTRACING_GEOMETRY_DESC>& GeometryDescs = accelerationStructureBuildTriangleGeometry->GeometryDescs;
		GeometryDescs = sequence<D3D12_RAYTRACING_GEOMETRY_DESC>(descriptor.geometries.count());

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& BuildInputs = accelerationStructureBuildTriangleGeometry->BuildInputs;
		BuildInputs.Type = translateAccelerationStructureType(descriptor.type);
		BuildInputs.Flags = translateAccelerationStructureBuildFlags(descriptor.flags);
		BuildInputs.NumDescs = static_cast<uint32>(GeometryDescs.count());
		BuildInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		// TODO(graphics/dx12/ext_hardware_ray_tracing/geometry-pointers): Expose both pGeometryDescs and ppGeometryDescs?
		BuildInputs.pGeometryDescs = &GeometryDescs[0];

		UINT GeometryIndex = 0;

		while (GeometryIndex < GeometryDescs.count()) {
			const AccelerationStructureTriangleGeometryDescriptor& geometry = descriptor.geometries[GeometryIndex];
			const BaseBufferView& VertexView = geometry.vertexView;
			const BaseBufferView& IndexView = geometry.indexView;
			const UINT VertexStride = VertexView.getStride();
			const UINT IndexStride = IndexView.getStride();

			D3D12_RAYTRACING_GEOMETRY_DESC& GeometryDesc = GeometryDescs[GeometryIndex];
			GeometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			GeometryDesc.Flags = translateGeometryFlags(geometry.flags);
			GeometryDesc.Triangles.Transform3x4 = 0;
			GeometryDesc.Triangles.IndexFormat = getIndexBufferFormat(IndexStride);
			GeometryDesc.Triangles.VertexFormat = getVertexInputAttributeFormat(geometry.vertexFormat);
			GeometryDesc.Triangles.IndexCount = static_cast<UINT>(IndexView.getSize()) / IndexStride;
			GeometryDesc.Triangles.VertexCount = static_cast<UINT>(VertexView.getSize()) / VertexStride;
			GeometryDesc.Triangles.IndexBuffer = pointer_cast<const DX12::Buffer*>(IndexView.getBuffer())->resource->GetGPUVirtualAddress() + IndexView.getOffset();
			GeometryDesc.Triangles.VertexBuffer.StartAddress = pointer_cast<const DX12::Buffer*>(VertexView.getBuffer())->resource->GetGPUVirtualAddress() + VertexView.getOffset();
			GeometryDesc.Triangles.VertexBuffer.StrideInBytes = VertexStride;

			GeometryIndex += 1;
		}

		return accelerationStructureBuildTriangleGeometry;
	}

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildTriangleGeometry> buildTriangleGeometry) {
		const DX12::Device* Device = static_cast<const DX12::Device*>(device);
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& BuildInputs = ref_cast<DX12::AccelerationStructureBuildTriangleGeometry>(buildTriangleGeometry)->BuildInputs;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO PrebuildInfo;

		Device->device5->GetRaytracingAccelerationStructurePrebuildInfo(&BuildInputs, &PrebuildInfo);

		AccelerationStructureBuildSizes buildSizes{};
		buildSizes.accelerationStructureSize = PrebuildInfo.ResultDataMaxSizeInBytes;
		buildSizes.scratchBuildSize = PrebuildInfo.ScratchDataSizeInBytes;
		buildSizes.scratchUpdateSize = PrebuildInfo.UpdateScratchDataSizeInBytes;

		return buildSizes;
	}
}