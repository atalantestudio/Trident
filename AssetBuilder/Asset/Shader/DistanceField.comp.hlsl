// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO: Change to a generic distance field shader.

struct FontBuffer {
	uint spreadFactor;
	uint outputGlyphSize;
};

struct GlyphBuffer {
	uint2 offset;
};

Texture2D<float> glyphImage : register(t0);
RWTexture2D<float> glyphMapImage : register(u0);
StructuredBuffer<FontBuffer> fontBuffer : register(t1);
StructuredBuffer<GlyphBuffer> glyphBuffer : register(t2);

[numthreads(8, 8, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 glyphImageCoordinate = globalInvocationIndex.xy;
	uint outputGlyphSize = fontBuffer[0].outputGlyphSize;
	uint2 glyphSize = uint2(outputGlyphSize, outputGlyphSize);

	if (glyphImageCoordinate.x >= glyphSize.x || glyphImageCoordinate.y >= glyphSize.y) {
		return;
	}

	const uint spreadFactor = fontBuffer[0].spreadFactor;
	const uint2 glyphOffset = glyphBuffer[0].offset;
	const uint spread = spreadFactor * outputGlyphSize;

	int2 inputTexelCoordinate = int2((glyphImageCoordinate - spreadFactor) * outputGlyphSize);
	float inputColor = glyphImage[inputTexelCoordinate];
	uint closestSquareDistance = max(1, spread * spread);

	if (spreadFactor > 0) {
		int2 sampleStart = inputTexelCoordinate - int(spread);
		int2 sampleEnd = inputTexelCoordinate + int(spread);
		int x;
		int y = sampleStart.y;

		while (y < sampleEnd.y) {
			x = sampleStart.x;

			while (x < sampleEnd.x) {
				int2 sampleCoordinate = int2(x, y);
				float r = glyphImage[sampleCoordinate];

				if (r == inputColor) {
					x += int(spreadFactor);

					continue;
				}

				uint2 power = uint2(pow(inputTexelCoordinate - sampleCoordinate, uint2(2, 2)));
				uint squareDistance = power.x + power.y;

				closestSquareDistance = min(closestSquareDistance, squareDistance);

				x += int(spreadFactor);
			}

			y += int(spreadFactor);
		}
	}

	float closestDistance = sqrt(closestSquareDistance) * ((inputColor - 0.5) * 2.0);
	float r = closestDistance / spread * 0.5 + 0.5;

	glyphMapImage[glyphOffset + glyphImageCoordinate] = r;
}