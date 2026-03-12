// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class PlatformLibrary {
		public:
			explicit PlatformLibrary(view<char8> libraryName);

			~PlatformLibrary();

			// TODO: Template argument for procedure type?
			void* loadProcedure(view<char8> procedureName) const;

		private:
			void* data;
	};
}