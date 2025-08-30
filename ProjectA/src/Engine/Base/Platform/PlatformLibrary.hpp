#pragma once

namespace ProjectA {
	class PlatformLibrary {
		public:
			explicit PlatformLibrary(std::string_view libraryName);

			~PlatformLibrary();

			// TODO(review): Template argument for procedure type?
			void* getProcedure(std::string_view procedureName) const;

		private:
			void* data;
	};
}