#ifndef _ML_CAMERA_HPP_
#define _ML_CAMERA_HPP_

// WIP

#include <modus_core/graphics/Transform.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	// https://github.com/CedricGuillemet/ImGuizmo/blob/master/example/main.cpp

	inline void frustum(float32 left, float32 right, float32 bottom, float32 top, float32 znear, float32 zfar, float32 * m16)
	{
		float32 temp, temp2, temp3, temp4;
		temp = 2.0f * znear;
		temp2 = right - left;
		temp3 = top - bottom;
		temp4 = zfar - znear;
		m16[0] = temp / temp2;
		m16[1] = 0.0;
		m16[2] = 0.0;
		m16[3] = 0.0;
		m16[4] = 0.0;
		m16[5] = temp / temp3;
		m16[6] = 0.0;
		m16[7] = 0.0;
		m16[8] = (right + left) / temp2;
		m16[9] = (top + bottom) / temp3;
		m16[10] = (-zfar - znear) / temp4;
		m16[11] = -1.0f;
		m16[12] = 0.0;
		m16[13] = 0.0;
		m16[14] = (-temp * zfar) / temp4;
		m16[15] = 0.0;
	}

	inline void perspective(float32 fovyInDegrees, float32 aspectRatio, float32 znear, float32 zfar, float32 * m16)
	{
		float32 ymax, xmax;
		ymax = znear * std::tanf(fovyInDegrees * 3.141592f / 180.0f);
		xmax = ymax * aspectRatio;
		frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
	}

	inline void cross(float32 const * a, float32 const * b, float32 * r)
	{
		r[0] = a[1] * b[2] - a[2] * b[1];
		r[1] = a[2] * b[0] - a[0] * b[2];
		r[2] = a[0] * b[1] - a[1] * b[0];
	}

	ML_NODISCARD inline float32 dot(float32 const * a, float32 const * b)
	{
		return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	}

	inline void normalize(float32 const * a, float32 * r)
	{
		float32 il = 1.f / (sqrtf(dot(a, a)) + FLT_EPSILON);
		r[0] = a[0] * il;
		r[1] = a[1] * il;
		r[2] = a[2] * il;
	}

	inline void look_at(float32 const * eye, float32 const * at, float32 const * up, float32 * m16)
	{
		float32 X[3], Y[3], Z[3], tmp[3];

		tmp[0] = eye[0] - at[0];
		tmp[1] = eye[1] - at[1];
		tmp[2] = eye[2] - at[2];

		normalize(tmp, Z);
		normalize(up, Y);

		cross(Y, Z, tmp);
		normalize(tmp, X);

		cross(Z, X, tmp);
		normalize(tmp, Y);

		m16[0] = X[0];
		m16[1] = Y[0];
		m16[2] = Z[0];
		m16[3] = 0.0f;
		m16[4] = X[1];
		m16[5] = Y[1];
		m16[6] = Z[1];
		m16[7] = 0.0f;
		m16[8] = X[2];
		m16[9] = Y[2];
		m16[10] = Z[2];
		m16[11] = 0.0f;
		m16[12] = -dot(X, eye);
		m16[13] = -dot(Y, eye);
		m16[14] = -dot(Z, eye);
		m16[15] = 1.0f;
	}

	inline void orthographic(float32 const l, float32 r, float32 b, float32 const t, float32 zn, float32 const zf, float32 * m16)
	{
		m16[0] = 2 / (r - l);
		m16[1] = 0.0f;
		m16[2] = 0.0f;
		m16[3] = 0.0f;
		m16[4] = 0.0f;
		m16[5] = 2 / (t - b);
		m16[6] = 0.0f;
		m16[7] = 0.0f;
		m16[8] = 0.0f;
		m16[9] = 0.0f;
		m16[10] = 1.0f / (zf - zn);
		m16[11] = 0.0f;
		m16[12] = (l + r) / (l - r);
		m16[13] = (t + b) / (b - t);
		m16[14] = zn / (zn - zf);
		m16[15] = 1.0f;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// CAMERA
namespace ml
{
	struct ML_NODISCARD camera
	{
		bool	is_perspective	{ true };
		mat4	proj			{},
				view			{ mat4::identity() };
		float32 fov				{ 27.f },
				view_width		{ 10.f },
				y_angle			{ 165.f / 180.f * 3.14159f },
				x_angle			{ 32.f / 180.f * 3.14159f },
				distance		{ 8.f };
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_CAMERA_HPP_