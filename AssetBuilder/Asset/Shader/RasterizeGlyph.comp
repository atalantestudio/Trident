#version 430

// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

struct Edge {
	// TODO: Pack x/y into 1x int.
	ivec2 start;
	ivec2 end;
	ivec2 control;

	bool curved;
};

layout(set = 0, binding = 0) buffer readonly Font {
	int lineHeight;
} font;

layout(set = 0, binding = 1) buffer readonly Glyph {
	uint edgeCount;
	Edge edges[];
} glyph;

layout(set = 0, binding = 2, r8) uniform writeonly image2D glyphImage;

void main() {
	uvec2 glyphImageCoordinate = gl_GlobalInvocationID.xy;
	uvec2 glyphImageSize = imageSize(glyphImage);

	if (glyphImageCoordinate.x >= glyphImageSize.x || glyphImageCoordinate.y >= glyphImageSize.y) {
		return;
	}

	vec2 p = vec2(glyphImageCoordinate) / glyphImageSize * font.lineHeight;
	int windingNumber = 0;
	uint edgeIndex = 0;

	// See https://gabormakesgames.com/blog_fonts_rasterization.html.
	while (edgeIndex < glyph.edgeCount) {
		Edge edge = glyph.edges[edgeIndex];
		vec2 a = edge.start;
		vec2 b = edge.end;

		// Test if the current scanline crosses the edge.
		if (a.y > p.y && p.y > b.y) {
			// Top-to-bottom edge.
			float crossProduct = (a.x - p.x) * (b.y - p.y) - (b.x - p.x) * (a.y - p.y);

			if (crossProduct < 0.0) {
				windingNumber += 1;
			}
		}
		else if (b.y > p.y && p.y > a.y) {
			// Bottom-to-top edge.
			float crossProduct = (a.x - p.x) * (b.y - p.y) - (b.x - p.x) * (a.y - p.y);

			if (crossProduct > 0.0) {
				windingNumber -= 1;
			}
		}

		if (edge.curved) {
			vec2 c = edge.control;

			// Calculate barycentric coordinates.
			float u = ((a.y - b.y) * (p.x - b.x) + (b.x - a.x) * (p.y - b.y)) / ((a.y - b.y) * (c.x - b.x) + (b.x - a.x) * (c.y - b.y));
			float v = ((b.y - c.y) * (p.x - b.x) + (c.x - b.x) * (p.y - b.y)) / ((a.y - b.y) * (c.x - b.x) + (b.x - a.x) * (c.y - b.y));
			float w = 1.0 - u - v;

			// Test if the current texel is inside the triangle.
			if (0.0 < u && u < 1.0 && 0.0 < v && v < 1.0 && 0.0 < w && w < 1.0) {
				vec2 controlToStart = a - c;
				vec2 controlToEnd = b - c;
				vec2 uw = u * vec2(0.5, 0.0) + w;
				float crossProduct = controlToStart.x * controlToEnd.y - controlToStart.y * controlToEnd.x;
				float coordinate = uw.x * uw.x - uw.y;

				if (crossProduct < 0.0) {
					coordinate = uw.y - uw.x * uw.x;
				}

				if (coordinate <= 0.0) {
					windingNumber += 1;
				}
				else {
					windingNumber -= 1;
				}
			}
		}

		edgeIndex += 1;
	}

	float r = float(windingNumber < 0);

	imageStore(glyphImage, ivec2(glyphImageCoordinate), vec4(r, 0.0, 0.0, 1.0));
}