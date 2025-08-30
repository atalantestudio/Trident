#include "pch.hpp"

#include "Backend/Win32/base.hpp"

namespace ProjectA {
	struct Win32PlatformLibraryData {
		HMODULE hModule;
	};

	PlatformLibrary::PlatformLibrary(std::string_view libraryName) {
		const LPCWSTR lpLibFileName = ConvertUTF8ToUTF16(libraryName.data());
		const HMODULE hModule = LoadLibrary(lpLibFileName);

		WIN32_ASSERT(hModule != nullptr);

		data = new Win32PlatformLibraryData();

		Win32PlatformLibraryData* libraryData = static_cast<Win32PlatformLibraryData*>(data);

		libraryData->hModule = hModule;
	}

	void* PlatformLibrary::getProcedure(std::string_view procedureName) const {
		static const HMODULE hModule = static_cast<const Win32PlatformLibraryData*>(data)->hModule;

		const FARPROC proc = GetProcAddress(hModule, procedureName.data());

		WIN32_ASSERT(proc != nullptr);

		return proc;
	}
}