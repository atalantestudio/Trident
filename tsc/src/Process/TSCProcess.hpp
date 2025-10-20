#pragma once

#include "Engine/Process/Process.hpp"

#include "../TSC/tsc.hpp"

namespace ProjectA {
	class TSCProcess : public Process {
		private:
			static constexpr uint32 PATH_ARGUMENT_INDEX = 0;

		public:
			explicit TSCProcess(const sequence<void*>& consoleArguments) {
				const char8* path = pointer_cast<const char8*>(consoleArguments[PATH_ARGUMENT_INDEX]);
				const sint32 code = tsc(path);

				ENGINE_INFO("TSC terminated with code [].", code);
			}
	};
}