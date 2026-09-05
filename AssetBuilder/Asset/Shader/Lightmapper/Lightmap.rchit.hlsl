// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Attributes {
	float2 barycentrics;
};

struct Payload {
	/* [[vk::location(0)]] */ float3 hitValue;
};

[shader("closesthit")]
void main(inout Payload payload, in Attributes attributes) {
	float3 barycentricCoordinates = float3(1.0 - attributes.barycentrics.x - attributes.barycentrics.y, attributes.barycentrics.x, attributes.barycentrics.y);

	payload.hitValue = barycentricCoordinates;
}