// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Payload {
	float3 hitValue;
};

[shader("miss")]
void main(inout Payload payload) {
	payload.hitValue = float3(1.0, 0.2, 0.0);
}