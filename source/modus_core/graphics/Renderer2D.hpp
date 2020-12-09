#ifndef _ML_RENDERER_2D_HPP_
#define _ML_RENDERER_2D_HPP_

#include <modus_core/graphics/RenderCommand.hpp>

// RENDERER 2D
namespace ml
{
	// renderer 2D
	struct ML_CORE_API renderer2d final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		struct ML_NODISCARD metrics final
		{
			uint32 draw_calls, quad_count;

			ML_NODISCARD auto get_vertex_count() const { return quad_count * 4u; }

			ML_NODISCARD auto get_index_count() const { return quad_count * 6u; }
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		renderer2d(allocator_type alloc = {}) noexcept;

		~renderer2d() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_metrics() const noexcept -> metrics const & { return m_metrics; }

		void reset_metrics() { m_metrics = metrics{}; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void draw_quad(float32 x, float32 y, float32 w, float32 h);

		void draw_quad(vec2 const & pos, vec2 const & size) noexcept { draw_quad(pos[0], pos[1], size[0], size[1]); }

		void draw_quad(float_rect const & bounds) noexcept { draw_quad(bounds[0], bounds[1], bounds[2], bounds[3]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		metrics m_metrics;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDERER_2D_HPP_