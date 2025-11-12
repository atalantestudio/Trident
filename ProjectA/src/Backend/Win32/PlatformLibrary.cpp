#include "pch.hpp"

#include "Backend/Win32/base.hpp"

namespace ProjectA {
	struct Win32PlatformLibraryData {
		HMODULE hLibModule;
	};

	PlatformLibrary::PlatformLibrary(const str8& libraryName) {
		const LPCWSTR lpLibFileName = ConvertUTF8ToUTF16(&libraryName[0]);
		const HMODULE hLibModule = LoadLibrary(lpLibFileName);

		free((void*) lpLibFileName);

		WIN32_ASSERT(hLibModule != nullptr);

		data = new Win32PlatformLibraryData();

		Win32PlatformLibraryData* libraryData = static_cast<Win32PlatformLibraryData*>(data);

		libraryData->hLibModule = hLibModule;
	}

	PlatformLibrary::~PlatformLibrary() {
		const HMODULE hLibModule = static_cast<const Win32PlatformLibraryData*>(data)->hLibModule;

		WIN32_ASSERT(FreeLibrary(hLibModule) != 0);
	}

	void* PlatformLibrary::getProcedure(const str8& procedureName) const {
		static const HMODULE hLibModule = static_cast<const Win32PlatformLibraryData*>(data)->hLibModule;

		const FARPROC proc = GetProcAddress(hLibModule, &procedureName[0]);

		WIN32_ASSERT(proc != nullptr);

		return proc;
	}
}