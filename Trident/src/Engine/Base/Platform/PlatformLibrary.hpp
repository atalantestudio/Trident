// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO(ci/gcc-include-guard)
// #pragma once
#if !defined(PLATFORM_LIBRARY_HPP)
#define PLATFORM_LIBRARY_HPP

namespace Trident {
	class PlatformLibrary {
		public:
			explicit PlatformLibrary(view<char8> libraryName);

			~PlatformLibrary();

			template<typename Procedure>
			Procedure loadProcedure(view<char8> procedureName) const {
				#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
					return function_cast<Procedure>(loadWin32Procedure(procedureName));
				#else
					#error "Unsupported operating system."

					return function_cast<Procedure>(nullptr);
				#endif
			}

		private:
			#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
				void* loadWin32Procedure(view<char8> procedureName) const;
			#endif

		private:
			void* data;
	};
}

#endif