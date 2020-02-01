#pragma once

// Various math and geometry stuff

#include <glm/glm.hpp>

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;

#include <string.h>
#include <type_traits>
template<typename IntType, unsigned int FractionBits>
class FixedPoint {
	static_assert(FractionBits < (sizeof(IntType) * 8), "Fraction bits must be less than the number of bits in IntType");
	static_assert(std::is_integral<IntType>::value, "IntType must be an integer type");
private:
	// TODO: Optimize using built-in instructions
	inline static int findLastBit(uint64_t a) {
		int n = 0;
		for (; a > 0; a >>= 1)
			n += 1;
		return n;
	}
	
	IntType value;
	inline FixedPoint(IntType value) : value(value) {}
public:
	inline FixedPoint(const FixedPoint<IntType, FractionBits>& other) : FixedPoint(other.value) {}
	/*inline FixedPoint(double other) {
		setDouble(other);
	};*/
	inline FixedPoint(float other) {
		setFloat(other);
	}

	// TODO: Initializer from any other FixedPoint type
	// inline FixedPoint(FixedPoint other) {};

	inline void setFloat(float num) {
		int32_t bits = *reinterpret_cast<uint32_t*>(&num);
		int32_t mantissa = bits & ((1 << 23) - 1) | (1 << 23);
		int exponent = (bits >> 23) & ((1 << 8) - 1);

		exponent -= 127 + 23 - FractionBits; // Get the shift value
		if (exponent > 0)
			value = (IntType)mantissa << exponent;
		else
			value = (IntType)mantissa >> (-exponent);
		value = (bits & (1 << 31)) ? -value : value;
	}

	inline float getFloat() {
		IntType num = value;
		bool sign = (num < 0);
		if (sign) num = -num;

		int exponent = findLastBit(num);
		if (exponent == 0)
			return 0.0f;

		int32_t mantissa;
		exponent -= 1 + 23;
		if (exponent > 0)
			mantissa = num >> exponent;
		else
			mantissa = num << (-exponent);

		mantissa &= ((1 << 23) - 1);
		exponent += 127 + 23 - FractionBits;

		int32_t ret = (sign ? (1 << 31) : 0) | (exponent << 23) | mantissa;
		return *(float*)(&ret);
	}

	inline FixedPoint operator=(const FixedPoint other) { value = other.value; return *this; }
	inline FixedPoint operator+(const FixedPoint other) { return FixedPoint(value + other.value); }
	inline FixedPoint operator-(const FixedPoint other) { return FixedPoint(value - other.value); }
	// TODO: Proper multiplcation/division
	inline FixedPoint operator*(const FixedPoint other) {
		constexpr int s1 = FractionBits >> 1;
		constexpr int s2 = (FractionBits + 1) >> 1;

		IntType value = (value >> s1) * (other.value >> s2);
		return FixedPoint(value);
	}
	inline FixedPoint operator/(const FixedPoint other) {
		constexpr int s1 = FractionBits >> 1;
		constexpr int s2 = (FractionBits + 1) >> 1;

		IntType value = (value << s1) / (other.value >> s2);
		return FixedPoint(value);
	}
};

typedef FixedPoint<int64_t, 16> fixed48_16;

struct Position {
	fixed48_16 x, y, z;
	inline Position(fixed48_16 x, fixed48_16 y, fixed48_16 z) : x(x), y(y), z(z) {};

	inline Position operator+(Position other) {
		return Position(x + other.x, y + other.y, z + other.z);
	}

	inline Vec3 operator-(Position other) {
		return Vec3((x - other.x).getFloat(), (y - other.y).getFloat(), (z - other.z).getFloat());
	}

	inline Position operator+(Vec3 other) {
		return Position(x + fixed48_16(other.x), y + fixed48_16(other.y), z + fixed48_16(other.z));
	}

	inline Position operator-(Vec3 other) {
		return Position(x - fixed48_16(other.x), y - fixed48_16(other.y), z - fixed48_16(other.z));
	}
};
