#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/SharedMemory/SharedMemory.hpp"

namespace ProjectA {
	struct Win32SharedMemoryData {
		HANDLE hFileMappingObject;
	};

	SharedMemory::SharedMemory(std::string_view name, uint64 size) {
		const LPCWSTR lpName = ConvertUTF8ToUTF16(name.data());

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
		Win32SharedMemoryData* sharedMemoryData = static_cast<Win32SharedMemoryData*>(data);

		WIN32_ASSERT(CloseHandle(sharedMemoryData->hFileMappingObject) != 0);

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
		WIN32_ASSERT(UnmapViewOfFile(data) != 0);
	}
}