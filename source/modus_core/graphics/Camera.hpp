#ifndef _ML_CAMERA_HPP_
#define _ML_CAMERA_HPP_

// WIP

// Source: https://github.com/CedricGuillemet/ImGuizmo/blob/master/example/main.cpp

#include <modus_core/detail/Memory.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/Color.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// UTILITY
namespace ml::util
{
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

// CAMERA
namespace ml
{
	// camera
	struct camera : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		camera() noexcept
			: m_is_ortho	{}
			, m_clear_flags	{ 1 }
			, m_background	{ colors::magenta }
			, m_proj		{ mat4::identity() }
			, m_view		{ mat4::identity() }
			, m_clip		{ { 0.001f, 1000.f }, { 1000.f, -1000.f } }
			, m_fov			{ 27.f, 10.f }
			, m_eye			{ 5, 5, 5 }
			, m_target		{ 0, 0, 0 }
			, m_up			{ 0, 1, 0 }
			, m_world_up	{ 0, 1, 0 }
		{
		}

		camera(camera const & other)
			: m_is_ortho	{ other.m_is_ortho }
			, m_clear_flags	{ other.m_clear_flags }
			, m_background	{ other.m_background }
			, m_proj		{ other.m_proj }
			, m_view		{ other.m_view }
			, m_fov			{}
			, m_clip		{}
			, m_eye			{ other.m_eye }
			, m_target		{ other.m_target }
			, m_up			{ other.m_up }
			, m_world_up	{ other.m_world_up }
		{
			std::memcpy(m_fov, other.m_fov, sizeof(m_fov));
			std::memcpy(m_clip, other.m_clip, sizeof(m_clip));
		}

		camera(camera && other) noexcept : camera{}
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
				std::swap(m_is_ortho	, other.m_is_ortho);
				std::swap(m_clear_flags	, other.m_clear_flags);
				std::swap(m_fov			, other.m_fov);
				std::swap(m_clip		, other.m_clip);

				m_proj		.swap(other.m_proj);
				m_view		.swap(other.m_view);
				m_background.swap(other.m_background);
				m_eye		.swap(other.m_eye);
				m_target	.swap(other.m_target);
				m_up		.swap(other.m_up);
				m_world_up	.swap(other.m_world_up);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void recalculate(vec2 const & resolution)
		{
			ML_assert((resolution[0] != 0.f) && (resolution[1] != 0.f));
			
			// projection
			if (!m_is_ortho)
			{
				util::perspective(
					m_fov[m_is_ortho],
					resolution[0] / resolution[1],
					m_clip[m_is_ortho][0],
					m_clip[m_is_ortho][1],
					m_proj);
			}
			else
			{
				auto const height{ m_fov[m_is_ortho] * resolution[1] / resolution[0] };
				
				util::orthographic(
					-m_fov[m_is_ortho],
					m_fov[m_is_ortho],
					-height,
					height,
					m_clip[m_is_ortho][0],
					m_clip[m_is_ortho][1],
					m_proj);
			}

			// view
			util::look_at(m_eye, m_target, m_up, m_view);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_clear_flags() const noexcept -> uint32
		{
			return m_clear_flags;
		}

		ML_NODISCARD auto get_background() const noexcept -> color const &
		{
			return m_background;
		}

		void set_clear_flags(uint32 value) noexcept
		{
			if (m_clear_flags != value)
			{
				m_clear_flags = value;
			}
		}

		void set_background(color const & value) noexcept
		{
			if (m_background != value)
			{
				m_background = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_proj_matrix() const noexcept -> mat4 const &
		{
			return m_proj;
		}

		ML_NODISCARD bool is_orthographic() const noexcept
		{
			return m_is_ortho;
		}

		ML_NODISCARD auto get_fov() const noexcept -> float32
		{
			return m_fov[m_is_ortho];
		}

		ML_NODISCARD auto get_clip() const noexcept -> vec2 const &
		{
			return m_clip[m_is_ortho];
		}

		void set_proj_matrix(mat4 const & value) noexcept
		{
			if (m_proj != value)
			{
				m_proj = value;
			}
		}

		void set_orthographic(bool value) noexcept
		{
			if (m_is_ortho != value)
			{
				m_is_ortho = value;
			}
		}

		void set_fov(float32 value) noexcept
		{
			if (m_fov[m_is_ortho] != value)
			{
				m_fov[m_is_ortho] = value;
			}
		}

		void set_clip(vec2 const & value) noexcept
		{
			if (m_clip[m_is_ortho] != value)
			{
				m_clip[m_is_ortho] = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_view_matrix() const noexcept -> mat4 const &
		{
			return m_view;
		}

		ML_NODISCARD auto get_eye() const noexcept -> vec3 const &
		{
			return m_eye;
		}

		ML_NODISCARD auto get_target() const noexcept -> vec3 const &
		{
			return m_target;
		}

		ML_NODISCARD auto get_up() const noexcept -> vec3 const &
		{
			return m_up;
		}

		ML_NODISCARD auto get_world_up() const noexcept -> vec3 const &
		{
			return m_world_up;
		}

		void set_view_matrix(mat4 const & value) noexcept
		{
			if (m_view != value)
			{
				m_view = value;
			}
		}

		void set_eye(vec3 const & value) noexcept
		{
			if (m_eye != value)
			{
				m_eye = value;
			}
		}

		void set_target(vec3 const & value) noexcept
		{
			if (m_target != value)
			{
				m_target = value;
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

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		color	m_background	; // 
		uint32	m_clear_flags	; // 

		bool	m_is_ortho		; // 
		mat4	m_proj			; // 
		mat4	m_view			; // 
		float32 m_fov[2]		; // 
		vec2	m_clip[2]		; // 

		vec3	m_eye			; // 
		vec3	m_target		; // 
		vec3	m_up			; // 
		vec3	m_world_up		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// CAMERA CONTROLLER
namespace ml
{
	// camera controller
	struct camera_controller
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		camera_controller()
			: m_camera	{}
			, m_position{ 5, 5, 5 }
			, m_yaw		{ 32.f / 180.f * 3.14159f }
			, m_pitch	{ 165.f / 180.f * 3.14159f }
			, m_roll	{ 0.f }
			, m_zoom	{ 8.f }
		{
		}

		camera_controller(camera_controller const & other)
			: m_camera	{ other.m_camera }
			, m_position{ other.m_position }
			, m_yaw		{ other.m_yaw }
			, m_pitch	{ other.m_pitch }
			, m_roll	{ other.m_roll }
			, m_zoom	{ other.m_zoom }
		{
		}

		camera_controller(camera_controller && other) noexcept
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		camera_controller & operator=(camera_controller const & other)
		{
			camera_controller temp{ other };
			this->swap(temp);
			return (*this);
		}

		camera_controller & operator=(camera_controller && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(camera_controller & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_camera	, other.m_camera);
				std::swap(m_position, other.m_position);
				std::swap(m_yaw		, other.m_yaw);
				std::swap(m_pitch	, other.m_pitch);
				std::swap(m_roll	, other.m_roll);
				std::swap(m_zoom	, other.m_zoom);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void recalculate(vec2 const & resolution)
		{
			ML_assert(m_camera);

			vec3 const eye
			{
				cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)) * m_zoom,
				sinf(glm::radians(m_pitch)) * m_zoom,
				sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch)) * m_zoom
			};

			m_camera->recalculate(resolution);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_camera() const noexcept -> camera *
		{
			return m_camera;
		}

		void set_camera(camera * value) noexcept
		{
			if (m_camera != value)
			{
				m_camera = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_position() const noexcept -> vec3 const &
		{
			return m_position;
		}

		void set_position(vec3 const & value) noexcept
		{
			if (m_position != value)
			{
				m_position = value;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
		camera *		m_camera	; // 
		vec3			m_position	; // 
		float32			m_yaw		; // 
		float32			m_pitch		; // 
		float32			m_roll		; // 
		float32			m_zoom		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_CAMERA_HPP_