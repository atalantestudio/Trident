#include "pch.hpp"

#include "Backend/Win32/base.hpp"

#include "Engine/API/Synchronization/Mutex.hpp"

namespace ProjectA {
	struct Win32MutexData {
		HANDLE hMutex;
	};

	Mutex::Mutex(const std::string& name) {
		DEBUG_ASSERT(name.size() < MAX_PATH);

		const LPSECURITY_ATTRIBUTES lpMutexAttributes = nullptr;
		const BOOL bInitialOwner = FALSE;
		const LPCWSTR lpName = ConvertUTF8ToUTF16(name.data());

		Win32MutexData* mutexData = new Win32MutexData();

		mutexData->hMutex = CreateMutex(lpMutexAttributes, bInitialOwner, lpName);

		free((void*) lpName);

		WIN32_ASSERT(mutexData->hMutex != nullptr);

		data = mutexData;
	}

	Mutex::~Mutex() {
		release();

		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);

		WIN32_ASSERT(CloseHandle(mutexData->hMutex) != 0);

		delete data;

		data = nullptr;
	}

	void Mutex::acquire() const {
		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);

		WIN32_ASSERT(WaitForSingleObject(mutexData->hMutex, INFINITE) == WAIT_OBJECT_0);
	}

	void Mutex::release() const {
		Win32MutexData* mutexData = static_cast<Win32MutexData*>(data);

		const DWORD event = ReleaseMutex(mutexData->hMutex);

		WIN32_ASSERT(event != WAIT_FAILED);
	}
}