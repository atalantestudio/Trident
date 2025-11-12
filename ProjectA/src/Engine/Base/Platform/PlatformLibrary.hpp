#pragma once

namespace ProjectA {
	class PlatformLibrary {
		public:
			explicit PlatformLibrary(const str8& libraryName);

			~PlatformLibrary();

			// TODO: Template argument for procedure type?
			void* getProcedure(const str8& procedureName) const;

		private:
			void* data;
	};
}