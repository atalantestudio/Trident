// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

struct Edge {
	// TODO: Pack x/y into 1x int.
	int2 start;
	int2 end;
	int2 control;

	bool curved;
};

ByteAddressBuffer fontBuffer : register(t0);
ByteAddressBuffer glyphBuffer : register(t1);
RWTexture2D<float> glyphImage : register(u0);

Edge loadEdge(uint edgeIndex) {
	static const uint stride = 32;

	Edge edge;
	edge.start = asint(glyphBuffer.Load(edgeIndex * stride + 0));
	edge.end = asint(glyphBuffer.Load(edgeIndex * stride + 8));
	edge.control = asint(glyphBuffer.Load(edgeIndex * stride + 16));
	edge.curved = glyphBuffer.Load(edgeIndex * stride + 24);

	return edge;
}

[numthreads(32, 32, 1)]
void main(uint3 globalInvocationIndex : SV_DispatchThreadID) {
	uint2 glyphImageCoordinate = globalInvocationIndex.xy;
	uint2 glyphImageSize;

	glyphImage.GetDimensions(glyphImageSize.x, glyphImageSize.y);

	if (glyphImageCoordinate.x >= glyphImageSize.x || glyphImageCoordinate.y >= glyphImageSize.y) {
		return;
	}

	int fontLineHeight = asint(fontBuffer.Load(0));
	uint glyphEdgeCount = glyphBuffer.Load(0);

	float2 p = float2(glyphImageCoordinate) / glyphImageSize * fontLineHeight;
	int windingNumber = 0;
	uint edgeIndex = 0;

	// See https://gabormakesgames.com/blog_fonts_rasterization.html.
	while (edgeIndex < glyphEdgeCount) {
		Edge edge = loadEdge(edgeIndex);
		float2 a = edge.start;
		float2 b = edge.end;

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
			float2 c = edge.control;

			// Calculate barycentric coordinates.
			float u = ((a.y - b.y) * (p.x - b.x) + (b.x - a.x) * (p.y - b.y)) / ((a.y - b.y) * (c.x - b.x) + (b.x - a.x) * (c.y - b.y));
			float v = ((b.y - c.y) * (p.x - b.x) + (c.x - b.x) * (p.y - b.y)) / ((a.y - b.y) * (c.x - b.x) + (b.x - a.x) * (c.y - b.y));
			float w = 1.0 - u - v;

			// Test if the current texel is inside the triangle.
			if (0.0 < u && u < 1.0 && 0.0 < v && v < 1.0 && 0.0 < w && w < 1.0) {
				float2 controlToStart = a - c;
				float2 controlToEnd = b - c;
				float2 uw = u * float2(0.5, 0.0) + w;
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

	glyphImage[glyphImageCoordinate] = r;
}