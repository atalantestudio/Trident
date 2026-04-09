// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class ScriptEntity {
		public:
			virtual ~ScriptEntity() {}

			virtual void onConstruct() {}

			virtual void onUpdate() {}

			virtual void onDestroy() {}
	};
}