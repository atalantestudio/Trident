#pragma once

namespace Trident {
	class PlatformLibrary {
		public:
			explicit PlatformLibrary(view<char8> libraryName);

			~PlatformLibrary();

			// TODO: Template argument for procedure type?
			void* getProcedure(view<char8> procedureName) const;

		private:
			void* data;
	};
}