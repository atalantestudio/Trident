// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Window/Win32/base.hpp"

#include "Engine/API/Synchronization/Mutex.hpp"

namespace Trident {
	struct Win32MutexData {
		HANDLE hMutex;
	};

	Mutex::Mutex(const std::string& name) {
		DEBUG_ASSERT(name.size() < MAX_PATH);

		const LPSECURITY_ATTRIBUTES lpMutexAttributes = nullptr;
		const BOOL bInitialOwner = FALSE;
		const LPCWSTR lpName = ConvertUTF8ToUTF16(name.data());

		Win32MutexData* mutexData = new Win32MutexData();

		mutexData->hMutex = CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);

		free((void*) lpName);

		WIN32_ASSERT(mutexData->hMutex != nullptr);

		data = mutexData;
	}

	Mutex::~Mutex() {
		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);
		BOOL result;

		release();

		result = CloseHandle(mutexData->hMutex);

		WIN32_ASSERT(result != 0);

		delete data;

		data = nullptr;
	}

	void Mutex::acquire() const {
		DWORD result;
		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);

		result = WaitForSingleObject(mutexData->hMutex, INFINITE);

		WIN32_ASSERT(result == WAIT_OBJECT_0);
	}

	void Mutex::release() const {
		DWORD result;
		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);

		result = ReleaseMutex(mutexData->hMutex);

		WIN32_ASSERT(result != WAIT_FAILED);
	}
}