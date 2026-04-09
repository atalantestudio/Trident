// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

namespace Trident {
	struct Win32PlatformLibraryData {
		HMODULE hLibModule;
	};

	PlatformLibrary::PlatformLibrary(view<char8> libraryName) {
		const LPCWSTR lpLibFileName = ConvertUTF8ToUTF16(&libraryName[0]);
		const HMODULE hLibModule = LoadLibrary(lpLibFileName);

		free((void*) lpLibFileName);

		WIN32_ASSERT(hLibModule != NULL);

		data = new Win32PlatformLibraryData();

		Win32PlatformLibraryData* libraryData = static_cast<Win32PlatformLibraryData*>(data);
		libraryData->hLibModule = hLibModule;
	}

	PlatformLibrary::~PlatformLibrary() {
		const HMODULE hLibModule = static_cast<const Win32PlatformLibraryData*>(data)->hLibModule;
		BOOL result;

		result = FreeLibrary(hLibModule);

		WIN32_ASSERT(result != 0);
	}

	void* PlatformLibrary::loadWin32Procedure(view<char8> procedureName) const {
		static const HMODULE hLibModule = static_cast<const Win32PlatformLibraryData*>(data)->hLibModule;

		FARPROC procedure = GetProcAddress(hLibModule, &procedureName[0]);

		WIN32_ASSERT(procedure != NULL);

		return function_cast<void*>(procedure);
	}
}