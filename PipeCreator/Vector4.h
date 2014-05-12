#pragma once


#include <stdexcept>
#include <iostream>

#define VECTOR4_RANGE_ERR "index must be in [0,4)"

class Vector4
{
protected:
	float vector[4];

public:

	static const int SIZE = 4;

	/*==============================*
	*         Constructors         *
	*==============================*/

	Vector4(float x, float y, float z, float w) {
		this->vector[0] = x;
		this->vector[1] = y;
		this->vector[2] = z;
		this->vector[3] = w;
	}

	Vector4() : Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}

	Vector4(float x, float y, float z) : Vector4(x, y, z, 0.0f) {}

	Vector4(const Vector4& v) : Vector4(v[0], v[1], v[2], v[3]) {}
	/*==============================*
	*      Setters and Getters     *
	*==============================*/

	// Set

	void set(float x, float y, float z, float w) {
		this->vector[0] = x;
		this->vector[1] = y;
		this->vector[2] = z;
		this->vector[3] = w;
	}

	void set(float x, float y, float z) {
		this->vector[0] = x;
		this->vector[1] = y;
		this->vector[2] = z;
	}

	void set(int index, float value) {
		if (index < 0 || index >= SIZE) throw std::out_of_range(VECTOR4_RANGE_ERR);
		else this->vector[index] = value;
	}

	// Get

	float get(int index) const {
		if (index < 0 || index >= SIZE) throw std::out_of_range(VECTOR4_RANGE_ERR);
		else return this->vector[index];
	}

	float& operator [](const int index) {
		if (index < 0 || index >= SIZE) throw std::out_of_range(VECTOR4_RANGE_ERR);
		else return this->vector[index];
	}

	float operator [](const int index) const {
		return this->get(index);
	}

	// X, Y, Z, W

	inline float x() const {
		return this->vector[0];
	}

	inline float y() const {
		return this->vector[1];
	}

	inline float z() const {
		return this->vector[2];
	}

	inline float w() const {
		return this->vector[3];
	}

	/*==============================*
	 *          Arithmetic          *
	 *==============================*/

	// Add

	Vector4& operator+=(const Vector4& v) {
		this->vector[0] += v[0];
		this->vector[1] += v[1];
		this->vector[2] += v[2];
		this->vector[3] += v[3];

		return *this;
	}

	void add(const Vector4& v) {
		(*this) += v;
	}

	// Subtract

	Vector4& operator-=(const Vector4& v) {
		this->vector[0] -= v[0];
		this->vector[1] -= v[1];
		this->vector[2] -= v[2];
		this->vector[3] -= v[3];

		return *this;
	}

	void subtract(const Vector4& v) {
		(*this) -= v;
	}

	// Multiply

	Vector4& operator*=(const float f) {
		this->vector[0] *= f;
		this->vector[1] *= f;
		this->vector[2] *= f;

		return *this;
	}

	// Cross Product

	static Vector4 cross(Vector4 u, const Vector4& v) {
		u.cross(v);
		return u;
	}

	void cross(const Vector4& v) {
		//calculate cross product
		float s1 = this->vector[1] * v[2] - this->vector[2] * v[1];
		float s2 = this->vector[2] * v[0] - this->vector[0] * v[2];
		float s3 = this->vector[0] * v[1] - this->vector[1] * v[0];

		//clobber
		this->vector[0] = s1;
		this->vector[1] = s2;
		this->vector[2] = s3;
	}

	// Dot Product

	static float dot(const Vector4& u, const Vector4& v) {
		return u.dot(v);
	}

	float dot(const Vector4& u) const {
		return this->vector[0] * u.vector[0] + this->vector[1] * u.vector[1] + this->vector[2] * u.vector[2];
	}

	// Angle Between

	static float angle(const Vector4& u, const Vector4& v) {
		return u.angle(v);
	}

	float angle(const Vector4& u) const {
		return acos(this->dot(u) / (this->length() * u.length()));
	}


	/*==============================*
	 *           Utility            *
	 *==============================*/

	static Vector4 dehomogenize(Vector4 v) {
		v.dehomogenize();
		return v;
	}

	void dehomogenize() {
		float factor = this->vector[3];
		this->vector[0] /= factor;
		this->vector[1] /= factor;
		this->vector[2] /= factor;
		this->vector[3] /= factor;
	}


	static Vector4 normalize(Vector4 v) {
		v.normalize();
		return v;
	}

	void normalize() {
		float mag = this->length();
		if (mag != 0) {
			this->vector[0] /= mag;
			this->vector[1] /= mag;
			this->vector[2] /= mag;
		}

	}

	// Negation

	Vector4 negate(Vector4 u) {
		u.negate();
		return u;
	}

	void negate() {
		*this *= -1.0f;
	}

	float length() const {
		return sqrt(this->lengthSquared());
	}

	float lengthSquared() const {
		return this->dot(*this);
	}


	void print() const {
		std::cout << "<" <<
			this->vector[0] << ", " <<
			this->vector[1] << ", " <<
			this->vector[2] << ", " <<
			this->vector[3] << ">" << std::endl;
	}


	static Vector4 lerp(float f, Vector4 u, Vector4 v) {
		v -= u;
		v *= f;
		u += v;
		return u;
	}
};

inline Vector4 operator+(Vector4 u, const Vector4& v) {
	u += v;
	return u;
}

inline Vector4 operator-(Vector4 u, const Vector4& v) {
	u -= v;
	return u;
}

inline Vector4 operator*(Vector4 u, const float f) {
	u *= f;
	return u;
}

inline Vector4 operator-(Vector4 u) {
	u.negate();
	return u;
}



