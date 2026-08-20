// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(base/float16): Move to Base submodule? Or the asset builder, since it's only used there?
	/// 16-bit floating-point IEEE 754 number.
	/// See https://stackoverflow.com/a/16171497/17136841 and https://stackoverflow.com/a/76816560/17136841.
	struct float16 {
		public:
			float16() = default;

			float16(uint16 a) :
				value(a)
			{}

			float16(float32 a) {
				static constexpr uint32 exp2_11 = 2048;
				static constexpr uint32 exp2_12 = 4096;
				static constexpr uint32 exp2_13 = 8192;
				static constexpr uint32 exp2_23 = 8388608;

				const uint32 b = *pointer_cast<const uint32*>(&a);

				const uint32 sign = b >> 31;
				uint8 exponent = static_cast<uint8>((b >> 23) & 0xff);
				uint32 significand = b & 0x7fffff;

				// NaN
				if (exponent == 0xff && significand != 0) {
					value |= 0b11111 << 10;
					value |= (significand >> 22) << 9;
					value |= (significand >> 12) & 0x1ff;

					return;
				}

				if (exponent == 0) {
					significand += 1;
				}
				else {
					significand += exp2_23;
				}

				sint8 signedExponent = static_cast<sint8>(exponent);

				signedExponent -= 127;
				signedExponent += 15;

				if (signedExponent < -11) {
					value |= sign << 15;

					return;
				}

				if (signedExponent < 1) {
					uint32 temp = significand << (1 - signedExponent);

					significand >>= (1 - signedExponent + 13);

					if (temp > exp2_23) {
						significand += 1;

						if (significand >= 1024) {
							value |= sign << 15;
							value |= 1 << 10;

							// TODO(base/float16): Finish and test implementation.
							DEBUG_ASSERT(false);

							return;
						}

						value |= sign << 15;
						value |= (significand >> 13) & 0x1ff;

						// TODO(base/float16): Finish and test implementation.
						DEBUG_ASSERT(false);

						return;
					}
				}

				exponent = static_cast<uint8>(signedExponent);

				const uint32 low13 = significand & 0x1fff;

				if (low13 > exp2_12 || low13 == exp2_12 && (significand >> 13) & 1) {
					significand += exp2_13;
					significand >>= 13;

					if (significand >= exp2_11) {
						exponent += 1;
						significand >>= 1;
					}
				}

				// +/-Infinity
				if (exponent > 30) {
					value |= sign << 15;
					value |= 0b11111 << 10;

					return;
				}

				value |= sign << 15;
				value |= (exponent & 0x1f) << 10;
				value |= (significand >> 13) & 0x3ff;
			}

			operator uint16() const {
				return value;
			}

			operator float32() const {
				// TODO(base/float16): Replace with a scalar conversion.
				const __m128i mi = _mm_set1_epi16(value);
				const __m128 m = _mm_cvtph_ps(mi);

				return _mm_cvtss_f32(m);
			}

		private:
			uint16 value = 0;
	};

	// TODO(test)
	/* float16 a = 0.0f; // OK
	float16 a = -0.0f; // OK
	float16 a = 1.0f; // OK
	float16 a = -1.0f; // OK
	float16 a = 3.140625f; // OK
	float16 a = 0.15625f; // OK
	float16 a = FLT_MAX;
	float16 a = FLT_MIN;
	float16 a = infinity<float32>();
	float16 a = -infinity<float32>();
	float16 a = lyah::nan<float32>(); // OK
	float16 a = length(normalized(vec2<float32>{})); // -nan(ind), OK */
}