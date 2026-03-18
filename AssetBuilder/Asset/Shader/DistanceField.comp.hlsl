// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO: Change to a generic distance field shader.

Texture2D<float> glyphImage : register(t0);
RWTexture2D<float> glyphMapImage : register(u0);
ByteAddressBuffer fontBuffer : register(t1);
ByteAddressBuffer glyphBuffer : register(t2);

[numthreads(8, 8, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 glyphImageCoordinate = globalInvocationIndex.xy;
	uint fontOutputGlyphSize = fontBuffer.Load(4);
	uint2 glyphSize = uint2(fontOutputGlyphSize, fontOutputGlyphSize);

	if (glyphImageCoordinate.x >= glyphSize.x || glyphImageCoordinate.y >= glyphSize.y) {
		return;
	}

	uint fontSpreadFactor = fontBuffer.Load(0);
	uint2 glyphOffset = glyphBuffer.Load2(0);

	const uint spread = fontSpreadFactor * fontOutputGlyphSize;

	int2 inputTexelCoordinate = int2((glyphImageCoordinate - fontSpreadFactor) * fontOutputGlyphSize);
	float inputColor = glyphImage[inputTexelCoordinate];
	uint closestSquareDistance = max(1, spread * spread);

	if (fontSpreadFactor > 0) {
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
					x += int(fontSpreadFactor);

					continue;
				}

				uint2 power = uint2(pow(inputTexelCoordinate - sampleCoordinate, uint2(2, 2)));
				uint squareDistance = power.x + power.y;

				closestSquareDistance = min(closestSquareDistance, squareDistance);

				x += int(fontSpreadFactor);
			}

			y += int(fontSpreadFactor);
		}
	}

	float closestDistance = sqrt(closestSquareDistance) * ((inputColor - 0.5) * 2.0);
	float r = closestDistance / spread * 0.5 + 0.5;

	glyphMapImage[glyphOffset + glyphImageCoordinate] = r;
}