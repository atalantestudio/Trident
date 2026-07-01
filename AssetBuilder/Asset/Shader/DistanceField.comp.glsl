#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

// TODO(asset-builder/sdf-generator/distance-field): Change to a generic distance field shader.

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(set = 0, binding = 0, r8) uniform readonly image2D glyphImage;

layout(set = 0, binding = 1, r8) uniform writeonly image2D glyphMapImage;

layout(set = 0, binding = 2) buffer readonly Font {
	uint spreadFactor;
	uint outputGlyphSize;
} font;

layout(set = 0, binding = 3) buffer readonly Glyph {
	uvec2 offset;
} glyph;

void main() {
	uvec2 glyphImageCoordinate = gl_GlobalInvocationID.xy;
	uvec2 glyphSize = uvec2(font.outputGlyphSize);

	if (glyphImageCoordinate.x >= glyphSize.x || glyphImageCoordinate.y >= glyphSize.y) {
		return;
	}

	const uint spread = font.spreadFactor * font.outputGlyphSize;

	ivec2 inputTexelCoordinate = ivec2((glyphImageCoordinate - font.spreadFactor) * font.outputGlyphSize);
	float inputColor = imageLoad(glyphImage, inputTexelCoordinate).r;
	uint closestSquareDistance = max(1, spread * spread);

	if (font.spreadFactor > 0) {
		ivec2 sampleStart = inputTexelCoordinate - int(spread);
		ivec2 sampleEnd = inputTexelCoordinate + int(spread);
		int x;
		int y = sampleStart.y;

		while (y < sampleEnd.y) {
			x = sampleStart.x;

			while (x < sampleEnd.x) {
				ivec2 sampleCoordinate = ivec2(x, y);
				float r = imageLoad(glyphImage, sampleCoordinate).r;

				if (r == inputColor) {
					x += int(font.spreadFactor);

					continue;
				}

				uvec2 power = uvec2(pow(inputTexelCoordinate - sampleCoordinate, uvec2(2)));
				uint squareDistance = power.x + power.y;

				closestSquareDistance = min(closestSquareDistance, squareDistance);

				x += int(font.spreadFactor);
			}

			y += int(font.spreadFactor);
		}
	}

	float closestDistance = sqrt(closestSquareDistance) * ((inputColor - 0.5) * 2.0);
	float r = closestDistance / spread * 0.5 + 0.5;

	imageStore(glyphMapImage, ivec2(glyph.offset + glyphImageCoordinate), vec4(r, 0.0, 0.0, 1.0));
}