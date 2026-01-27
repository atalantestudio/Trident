// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

#include "Engine/API/SharedMemory/SharedMemory.hpp"

namespace Trident {
	struct Win32SharedMemoryData {
		HANDLE hFileMappingObject;
	};

	SharedMemory::SharedMemory(view<char8> name, uint64 size) {
		const LPCWSTR lpName = ConvertUTF8ToUTF16(&name[0]);

		Win32SharedMemoryData* sharedMemoryData = new Win32SharedMemoryData();

		// Try to open file mapping.
		sharedMemoryData->hFileMappingObject = OpenFileMappingW(
			FILE_MAP_WRITE,
			FALSE,
			lpName
		);

		if (sharedMemoryData->hFileMappingObject == nullptr) {
			// File mapping doesn't exist; create it.
			sharedMemoryData->hFileMappingObject = CreateFileMappingW(
				INVALID_HANDLE_VALUE,
				nullptr,
				PAGE_READWRITE,
				size >> 32,
				static_cast<DWORD>(size),
				lpName
			);

			WIN32_ASSERT(sharedMemoryData->hFileMappingObject != nullptr);
		}

		free((void*) lpName);

		data = sharedMemoryData;
	}

	SharedMemory::~SharedMemory() {
		BOOL result;
		Win32SharedMemoryData* sharedMemoryData = static_cast<Win32SharedMemoryData*>(data);

		result = CloseHandle(sharedMemoryData->hFileMappingObject);

		WIN32_ASSERT(result != 0);

		delete data;

		data = nullptr;
	}

	void* SharedMemory::map(uint64 offset, uint64 size) const {
		Win32SharedMemoryData* sharedMemoryData = static_cast<Win32SharedMemoryData*>(data);

		void* mappedRange = MapViewOfFile(
			sharedMemoryData->hFileMappingObject,
			FILE_MAP_WRITE,
			offset >> 32,
			static_cast<DWORD>(offset),
			size
		);

		WIN32_ASSERT(mappedRange != nullptr);

		return mappedRange;
	}

	void SharedMemory::unmap(void* data) const {
		BOOL result;

		result = UnmapViewOfFile(data);

		WIN32_ASSERT(result != 0);
	}
}