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
#include "Engine/Adapter/Graphics/DX12/DeviceExtension/EXT_hardware_ray_tracing/RayTracingPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/IndexFormat.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
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
		BuildDesc.SourceAccelerationStructureData = 0;

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

			D3D12_RAYTRACING_GEOMETRY_DESC& GeometryDesc = GeometryDescs[GeometryIndex];
			GeometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			GeometryDesc.Flags = translateGeometryFlags(geometry.flags);

			D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC& Triangles = GeometryDesc.Triangles;
			Triangles.Transform3x4 = 0;
			Triangles.IndexFormat = translateIndexFormat(geometry.indexFormat);
			Triangles.VertexFormat = getVertexInputAttributeFormat(geometry.vertexFormat);
			Triangles.IndexCount = static_cast<UINT>(IndexView.getSize()) / getIndexSize(geometry.indexFormat);
			Triangles.VertexCount = static_cast<UINT>(VertexView.getSize()) / VertexStride;
			Triangles.IndexBuffer = pointer_cast<const DX12::Buffer*>(IndexView.getBuffer())->resource->GetGPUVirtualAddress() + IndexView.getOffset();
			Triangles.VertexBuffer.StartAddress = pointer_cast<const DX12::Buffer*>(VertexView.getBuffer())->resource->GetGPUVirtualAddress() + VertexView.getOffset();
			Triangles.VertexBuffer.StrideInBytes = VertexStride;

			GeometryIndex += 1;
		}

		return accelerationStructureBuildGeometry;
	}

	ref<Trident::RayTracingPipeline> createRayTracingPipeline(const Trident::Device* device, const RayTracingPipelineDescriptor& descriptor) {
		static constexpr UINT BaseSubobjectCount = 4;

		const DX12::Device* dxDevice = static_cast<const DX12::Device*>(device);
		const ref<DX12::PipelineLayout> dxPipelineLayout = ref_cast<DX12::PipelineLayout>(descriptor.layout);

		ref<DX12::RayTracingPipeline> rayTracingPipeline = allocateRef<DX12::RayTracingPipeline>();
		rayTracingPipeline->device = device;
		rayTracingPipeline->exportNames = sequence<const WCHAR*>(descriptor.shaderModules.count());
		rayTracingPipeline->rootSignature = dxPipelineLayout->rootSignature;

		D3D12_GLOBAL_ROOT_SIGNATURE GlobalRootSignature{};
		GlobalRootSignature.pGlobalRootSignature = dxPipelineLayout->rootSignature;

		D3D12_RAYTRACING_SHADER_CONFIG RaytracingShaderConfig{};
		RaytracingShaderConfig.MaxPayloadSizeInBytes = descriptor.maxRayPayloadSize;
		RaytracingShaderConfig.MaxAttributeSizeInBytes = descriptor.maxRayAttributeSize;

		D3D12_RAYTRACING_PIPELINE_CONFIG RaytracingPipelineConfig{};
		RaytracingPipelineConfig.MaxTraceRecursionDepth = descriptor.maxRayRecursionDepth;

		D3D12_HIT_GROUP_DESC HitGroupDesc{};
		HitGroupDesc.HitGroupExport = L"HitGroup";
		HitGroupDesc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		HitGroupDesc.AnyHitShaderImport = nullptr;
		HitGroupDesc.ClosestHitShaderImport = nullptr;
		HitGroupDesc.IntersectionShaderImport = nullptr;

		sequence<LPWSTR> RenamedExportNames(descriptor.shaderModules.count());
		sequence<D3D12_EXPORT_DESC> ExportDescs(descriptor.shaderModules.count());
		sequence<D3D12_DXIL_LIBRARY_DESC> DXILLibraryDescs(descriptor.shaderModules.count());
		sequence<D3D12_STATE_SUBOBJECT> Subobjects(BaseSubobjectCount + descriptor.shaderModules.count());

		D3D12_STATE_SUBOBJECT& GlobalRootSignatureSubobject = Subobjects[0];
		GlobalRootSignatureSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
		GlobalRootSignatureSubobject.pDesc = &GlobalRootSignature;

		D3D12_STATE_SUBOBJECT& RaytracingShaderConfigSubobject = Subobjects[1];
		RaytracingShaderConfigSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
		RaytracingShaderConfigSubobject.pDesc = &RaytracingShaderConfig;

		D3D12_STATE_SUBOBJECT& RaytracingPipelineConfigSubobject = Subobjects[2];
		RaytracingPipelineConfigSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
		RaytracingPipelineConfigSubobject.pDesc = &RaytracingPipelineConfig;

		D3D12_STATE_SUBOBJECT& HitGroupDescSubobject = Subobjects[3];
		HitGroupDescSubobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
		HitGroupDescSubobject.pDesc = &HitGroupDesc;

		UINT ShaderModuleIndex = 0;

		while (ShaderModuleIndex < descriptor.shaderModules.count()) {
			const ref<DX12::ShaderModule> ShaderModule = ref_cast<DX12::ShaderModule>(descriptor.shaderModules[ShaderModuleIndex]);

			RenamedExportNames[ShaderModuleIndex] = ConvertUTF8ToUTF16(&ShaderModule->entryPoint[0]);

			LPCWSTR* ExportName = &rayTracingPipeline->exportNames[ShaderModuleIndex];

			D3D12_EXPORT_DESC& ExportDesc = ExportDescs[ShaderModuleIndex];
			ExportDesc.ExportToRename = RenamedExportNames[ShaderModuleIndex];
			ExportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

			switch (ShaderModule->stage) {
				case ShaderStage::RAY_GENERATION: {
					*ExportName = L"Generation";

					ExportDesc.Name = L"Generation";

					break;
				}
				case ShaderStage::RAY_CLOSEST_HIT: {
					*ExportName = L"HitGroup";

					ExportDesc.Name = L"ClosestHit";
					HitGroupDesc.ClosestHitShaderImport = ExportDesc.Name;

					break;
				}
				case ShaderStage::RAY_MISS: {
					*ExportName = L"Miss";

					ExportDesc.Name = L"Miss";

					break;
				}
				case ShaderStage::RAY_ANY_HIT: {
					*ExportName = L"HitGroup";

					ExportDesc.Name = L"AnyHit";
					HitGroupDesc.AnyHitShaderImport = ExportDesc.Name;

					break;
				}
				case ShaderStage::RAY_INTERSECTION: {
					*ExportName = L"HitGroup";

					ExportDesc.Name = L"Intersection";
					HitGroupDesc.IntersectionShaderImport = ExportDesc.Name;

					break;
				}
				default: {
					ADAPTER_ASSERT(false, "Unsupported shader stage.");

					break;
				}
			}

			D3D12_DXIL_LIBRARY_DESC& DXILLibraryDesc = DXILLibraryDescs[ShaderModuleIndex];
			DXILLibraryDesc.DXILLibrary = ShaderModule->bytecode;
			DXILLibraryDesc.NumExports = 1;
			DXILLibraryDesc.pExports = &ExportDesc;

			D3D12_STATE_SUBOBJECT& Subobject = Subobjects[BaseSubobjectCount + ShaderModuleIndex];
			Subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
			Subobject.pDesc = &DXILLibraryDesc;

			ShaderModuleIndex += 1;
		}

		D3D12_STATE_OBJECT_DESC Desc{};
		Desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		Desc.NumSubobjects = static_cast<UINT>(Subobjects.count());
		Desc.pSubobjects = &Subobjects[0];

		HRESULT result;

		result = dxDevice->device5->CreateStateObject(&Desc, IID_PPV_ARGS(&rayTracingPipeline->stateObject));

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to create state object.");

		result = rayTracingPipeline->stateObject->QueryInterface(&rayTracingPipeline->stateObjectProperties);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to query state object properties interface.");

		for (LPWSTR ExportName : RenamedExportNames) {
			free(ExportName);
		}

		return rayTracingPipeline;
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

	uint64 getBufferDeviceAddress(const Trident::Device* device, ref<Trident::Buffer> buffer) {
		const ref<DX12::Buffer> dxBuffer = ref_cast<DX12::Buffer>(buffer);

		return dxBuffer->resource->GetGPUVirtualAddress();
	}
}