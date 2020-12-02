#ifndef _ML_CAMERA_HPP_
#define _ML_CAMERA_HPP_

// WIP

#include <modus_core/detail/Memory.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/Color.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::util
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
		util::frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
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
		float32 il = 1.f / (std::sqrtf(util::dot(a, a)) + FLT_EPSILON);
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

		util::normalize(tmp, Z);
		util::normalize(up, Y);

		util::cross(Y, Z, tmp);
		util::normalize(tmp, X);

		util::cross(Z, X, tmp);
		util::normalize(tmp, Y);

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

	inline void orthographic(float32 l, float32 r, float32 b, float32 t, float32 zn, float32 const zf, float32 * m16)
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

// TEST CAMERA
namespace ml
{
	struct ML_NODISCARD test_camera
	{
		bool	is_perspective	{ true };
		mat4	proj			{ mat4::identity() },
				view			{ mat4::identity() };
		float32 fov				{ 27.f },
				ortho_width		{ 10.f },
				zoom_level		{ 8.f },
				y_angle			{ 165.f / 180.f * 3.14159f },
				x_angle			{ 32.f / 180.f * 3.14159f };

		void update(vec2 const & resolution)
		{
			if (is_perspective)
			{
				util::perspective(
					fov,
					resolution[0] / resolution[1],
					0.01f,
					100.f,
					proj);
			}
			else
			{
				auto const view_height{ ortho_width * resolution[1] / resolution[0] };
				util::orthographic(
					-ortho_width,
					ortho_width,
					-view_height,
					view_height,
					1000.f,
					-1000.f,
					proj);
			}
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// CAMERA
namespace ml
{
	struct camera
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		camera() noexcept
			: m_clear_color	{ colors::magenta }
			, m_clear_flags	{ 1 }
			, m_proj		{ mat4::identity() }
			, m_view		{ mat4::identity() }
			, m_position	{ 0, 0, 0 }
			, m_forward		{ 0, 0, 1 }
			, m_right		{ 1, 0, 0 }
			, m_up			{ 0, 1, 0 }
			, m_world_up	{ 0, 1, 0 }
			, m_clip		{ 0.001f, 1000.f }
			, m_fov			{ 27.f }
			, m_yaw			{ 32.f / 180.f * 3.14159f }
			, m_pitch		{ 165.f / 180.f * 3.14159f }
			, m_roll		{ 0.f }
			, m_zoom		{ 8.f }
		{
		}

		camera(camera const & other)
			: m_clear_color	{ other.m_clear_color }
			, m_clear_flags	{ other.m_clear_flags }
			, m_proj		{ other.m_proj }
			, m_view		{ other.m_view }
			, m_position	{ other.m_position }
			, m_forward		{ other.m_forward }
			, m_right		{ other.m_right }
			, m_up			{ other.m_up }
			, m_world_up	{ other.m_world_up }
			, m_clip		{ other.m_clip }
			, m_fov			{ other.m_fov }
			, m_yaw			{ other.m_yaw }
			, m_pitch		{ other.m_pitch }
			, m_roll		{ other.m_roll }
			, m_zoom		{ other.m_zoom }
		{
		}

		camera(camera && other) noexcept
			: camera{}
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		camera & operator=(camera const & other)
		{
			camera temp{ other };
			this->swap(temp);
			return (*this);
		}

		camera & operator=(camera && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(camera & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_proj		, other.m_proj);
				std::swap(m_clear_color	, other.m_clear_color);
				std::swap(m_clear_flags	, other.m_clear_flags);
				std::swap(m_position	, other.m_position);
				std::swap(m_forward		, other.m_forward);
				std::swap(m_up			, other.m_up);
				std::swap(m_world_up	, other.m_world_up);
				std::swap(m_fov			, other.m_fov);
				std::swap(m_clip		, other.m_clip);
				std::swap(m_yaw			, other.m_yaw);
				std::swap(m_pitch		, other.m_pitch);
				std::swap(m_roll		, other.m_roll);
				std::swap(m_zoom		, other.m_zoom);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void recalculate(vec2 const & resolution)
		{
			util::perspective(
				m_fov,
				resolution[0] / resolution[1],
				m_clip[0],
				m_clip[1],
				m_proj);

			util::look_at(m_position, m_position + m_forward, m_up, m_view);

			//vec3 eye
			//{
			//	cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)) * m_zoom,
			//	sinf(glm::radians(m_pitch)) * m_zoom,
			//	sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)) * m_zoom
			//};
			//vec3 at{ 0.f, 0.f, 0.f };
			//util::look_at(eye, at, m_up, m_view);
		}

		ML_NODISCARD auto get_proj() const noexcept -> mat4 const &
		{
			return m_proj;
		}

		ML_NODISCARD auto get_view() const noexcept -> mat4 const &
		{
			return m_view;
		}

		ML_NODISCARD auto set_proj(mat4 const & value) noexcept
		{
			if (m_proj != value)
			{
				m_proj = value;
			}
		}

		ML_NODISCARD auto set_view(mat4 const & value) noexcept
		{
			if (m_view != value)
			{
				m_view = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_clear_color() const noexcept -> color const &
		{
			return m_clear_color;
		}

		ML_NODISCARD auto get_clear_flags() const noexcept -> uint32
		{
			return m_clear_flags;
		}

		ML_NODISCARD auto get_position() const noexcept -> vec3 const &
		{
			return m_position;
		}

		ML_NODISCARD auto get_forward() const noexcept -> vec3 const &
		{
			return m_forward;
		}

		ML_NODISCARD auto get_right() const noexcept -> vec3 const &
		{
			return m_right;
		}

		ML_NODISCARD auto get_up() const noexcept -> vec3 const &
		{
			return m_up;
		}

		ML_NODISCARD auto get_world_up() const noexcept -> vec3 const &
		{
			return m_world_up;
		}

		ML_NODISCARD auto get_fov() const noexcept -> float32
		{
			return m_fov;
		}

		ML_NODISCARD auto get_clip() const noexcept -> vec2 const &
		{
			return m_clip;
		}

		ML_NODISCARD auto get_yaw() const noexcept -> float32
		{
			return m_yaw;
		}

		ML_NODISCARD auto get_pitch() const noexcept -> float32
		{
			return m_pitch;
		}

		ML_NODISCARD auto get_roll() const noexcept -> float32
		{
			return m_roll;
		}

		ML_NODISCARD auto get_zoom() const noexcept -> float32
		{
			return m_zoom;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void set_clear_color(color const & value) noexcept
		{
			if (m_clear_color != value)
			{
				m_clear_color = value;
			}
		}

		void set_clear_flags(uint32 value) noexcept
		{
			if (m_clear_flags != value)
			{
				m_clear_flags = value;
			}
		}

		void set_position(vec3 const & value) noexcept
		{
			if (m_position != value)
			{
				m_position = value;
			}
		}

		void set_forward(vec3 const & value) noexcept
		{
			if (m_forward != value)
			{
				m_forward = value;
			}
		}

		void set_up(vec3 const & value) noexcept
		{
			if (m_up != value)
			{
				m_up = value;
			}
		}

		void set_world_up(vec3 const & value) noexcept
		{
			if (m_world_up != value)
			{
				m_world_up = value;
			}
		}

		void set_fov(float32 value) noexcept
		{
			if (m_fov != value)
			{
				m_fov = value;
			}
		}

		void set_clip(vec2 const & value) noexcept
		{
			if (m_clip != value)
			{
				m_clip = value;
			}
		}

		void set_yaw(float32 value) noexcept
		{
			if (m_yaw != value)
			{
				m_yaw = value;
			}
		}

		void set_pitch(float32 value) noexcept
		{
			if (m_pitch != value)
			{
				m_pitch = value;
			}
		}

		void set_roll(float32 value) noexcept
		{
			if (m_roll != value)
			{
				m_roll = value;
			}
		}

		void set_zoom(float32 value) noexcept
		{
			if (m_zoom != value)
			{
				m_zoom = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		color	m_clear_color	; // 
		uint32	m_clear_flags	; // 
		
		mat4	m_proj			; // 
		mat4	m_view			; // 
		vec3	m_position		; // 
		vec3	m_forward		; // 
		vec3	m_right			; // 
		vec3	m_up			; // 
		vec3	m_world_up		; // 
		float32 m_fov			; // 
		vec2	m_clip			; // 

		float32	m_yaw			; // 
		float32	m_pitch			; // 
		float32	m_roll			; // 
		float32	m_zoom			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	struct camera_controller
	{

	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_CAMERA_HPP_