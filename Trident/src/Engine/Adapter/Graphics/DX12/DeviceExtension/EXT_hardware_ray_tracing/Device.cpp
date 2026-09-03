// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Buffer.hpp"
#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructure.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/AccelerationStructureBuildGeometry.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/GeometryFlags.hpp"
#include "Engine/Adapter/Graphics/DX12/Format.hpp"
#include "Engine/Adapter/Graphics/DX12/VertexInputAttributeFormat.hpp"

namespace Trident::DX12 {
	ref<Trident::AccelerationStructure> createAccelerationStructure(const Trident::Device* device, const AccelerationStructureDescriptor& descriptor) {
		const BaseBufferView view = descriptor.view;

		ref<DX12::AccelerationStructure> accelerationStructure = allocateRef<DX12::AccelerationStructure>();
		accelerationStructure->device = device;

		// TODO(graphics/dx12/ext_hardware_ray_tracing/geometry-flags): There's no way to set D3D12_RAYTRACING_GEOMETRY_FLAGS in D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC& BuildDesc = accelerationStructure->BuildDesc;
		BuildDesc.DestAccelerationStructureData = pointer_cast<const DX12::Buffer*>(view.getBuffer())->resource->GetGPUVirtualAddress() + view.getOffset();
		BuildDesc.Inputs = ref_cast<DX12::AccelerationStructureBuildGeometry>(descriptor.buildGeometry)->BuildInputs;
		BuildDesc.SourceAccelerationStructureData = NULL;

		return accelerationStructure;
	}

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildInstanceGeometry(const Trident::Device* device, const AccelerationStructureBuildInstanceGeometryDescriptor& descriptor) {
		ref<DX12::AccelerationStructureBuildGeometry> accelerationStructureBuildGeometry = allocateRef<DX12::AccelerationStructureBuildGeometry>();

		const BaseBufferView& instanceView = descriptor.instanceView;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& BuildInputs = accelerationStructureBuildGeometry->BuildInputs;
		BuildInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		BuildInputs.Flags = translateAccelerationStructureBuildFlags(descriptor.flags);
		BuildInputs.NumDescs = descriptor.instanceCount;
		BuildInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		BuildInputs.InstanceDescs = pointer_cast<const DX12::Buffer*>(instanceView.getBuffer())->resource->GetGPUVirtualAddress() + instanceView.getOffset();

		return accelerationStructureBuildGeometry;
	}

	ref<Trident::AccelerationStructureBuildGeometry> createAccelerationStructureBuildTriangleGeometry(const Trident::Device* device, const AccelerationStructureBuildTriangleGeometryDescriptor& descriptor) {
		ref<DX12::AccelerationStructureBuildGeometry> accelerationStructureBuildGeometry = allocateRef<DX12::AccelerationStructureBuildGeometry>();

		sequence<D3D12_RAYTRACING_GEOMETRY_DESC>& GeometryDescs = accelerationStructureBuildGeometry->GeometryDescs;
		GeometryDescs = sequence<D3D12_RAYTRACING_GEOMETRY_DESC>(descriptor.geometries.count());

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& BuildInputs = accelerationStructureBuildGeometry->BuildInputs;
		BuildInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		BuildInputs.Flags = translateAccelerationStructureBuildFlags(descriptor.flags);
		BuildInputs.NumDescs = static_cast<uint32>(GeometryDescs.count());
		BuildInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
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

			D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& Triangles = GeometryDesc.Triangles;
			Triangles.Transform3x4 = 0;
			Triangles.IndexFormat = getIndexBufferFormat(IndexStride);
			Triangles.VertexFormat = getVertexInputAttributeFormat(geometry.vertexFormat);
			Triangles.IndexCount = static_cast<UINT>(IndexView.getSize()) / IndexStride;
			Triangles.VertexCount = static_cast<UINT>(VertexView.getSize()) / VertexStride;
			Triangles.IndexBuffer = pointer_cast<const DX12::Buffer*>(IndexView.getBuffer())->resource->GetGPUVirtualAddress() + IndexView.getOffset();
			Triangles.VertexBuffer.StartAddress = pointer_cast<const DX12::Buffer*>(VertexView.getBuffer())->resource->GetGPUVirtualAddress() + VertexView.getOffset();
			Triangles.VertexBuffer.StrideInBytes = VertexStride;

			GeometryIndex += 1;
		}

		return accelerationStructureBuildGeometry;
	}

	ref<Trident::RayTracingPipeline> createRayTracingPipeline(const Trident::Device* device, const RayTracingPipelineDescriptor& descriptor) {
		// TODO(graphics/dx12/ext_hardware_ray_tracing/ray-tracing-pipeline)
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	AccelerationStructureBuildSizes getAccelerationStructureBuildSizes(const Trident::Device* device, ref<Trident::AccelerationStructureBuildGeometry> buildGeometry) {
		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(device);
		const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& BuildInputs = ref_cast<DX12::AccelerationStructureBuildGeometry>(buildGeometry)->BuildInputs;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO PrebuildInfo;

		dxDevice->device5->GetRaytracingAccelerationStructurePrebuildInfo(&BuildInputs, &PrebuildInfo);

		AccelerationStructureBuildSizes buildSizes{};
		buildSizes.accelerationStructureSize = PrebuildInfo.ResultDataMaxSizeInBytes;
		buildSizes.scratchBuildSize = PrebuildInfo.ScratchDataSizeInBytes;
		buildSizes.scratchUpdateSize = PrebuildInfo.UpdateScratchDataSizeInBytes;

		return buildSizes;
	}
}