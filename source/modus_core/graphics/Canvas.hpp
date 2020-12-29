#ifndef _ML_CANVAS_HPP_
#define _ML_CANVAS_HPP_

// WIP

#include <modus_core/graphics/Camera.hpp>
#include <modus_core/graphics/Viewport.hpp>

namespace ml
{
	// quad vertex
	struct quad_vertex final
	{
		vec3	position		; // 
		vec4	color			; // 
		vec2	texcoord		; // 
		float32	texture_index	; // 
		float32	tiling_factor	; // 
	};

	// canvas
	struct ML_CORE_API canvas : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		struct ML_NODISCARD metrics final
		{
			uint32 draw_calls, quad_count;

			ML_NODISCARD auto get_vertex_count() const { return quad_count * 4u; }

			ML_NODISCARD auto get_index_count() const { return quad_count * 6u; }
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		canvas(allocator_type alloc = {}) noexcept
		{
		}
		
		canvas(canvas const & other, allocator_type alloc = {})
		{
		}
		
		canvas(canvas && other, allocator_type alloc = {}) noexcept : canvas{ alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		canvas & operator=(canvas const & other)
		{
			canvas temp{ other };
			this->swap(temp);
			return (*this);
		}
		
		canvas & operator=(canvas && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}
		
		void swap(canvas & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_metrics, other.m_metrics);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool initialize(gfx::render_device * device);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_metrics() noexcept -> metrics const & { return m_metrics; }

		void reset_metrics() noexcept { m_metrics = {}; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_quad(float32 x, float32 y, float32 w, float32 h, float32 rotation = 0.f);

		void draw_quad(vec2 const & pos, vec2 const & size, float32 rotation = 0.f);

		void draw_quad(float_rect const & bounds, float32 rotation = 0.f);

		void draw_quad(mat4 const & m);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		metrics m_metrics; // 

		ref<gfx::vertexarray>	m_va; // 
		ref<gfx::vertexbuffer>	m_vb; // 
		ref<gfx::program>		m_shader; // 
		ref<gfx::texture2d>		m_texture; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CANVAS_HPP_