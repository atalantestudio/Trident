// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "Lightmap.hlsl"

[shader("miss")]
void main(inout Hit hit) {
	hit.t = -1.0;
}