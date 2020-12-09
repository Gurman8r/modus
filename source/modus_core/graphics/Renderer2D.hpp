#ifndef _ML_RENDERER_2D_HPP_
#define _ML_RENDERER_2D_HPP_

// WIP

#include <modus_core/graphics/Camera.hpp>
#include <modus_core/graphics/RenderCommand.hpp>

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

	// renderer 2D
	struct ML_CORE_API renderer2d final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		struct ML_NODISCARD metrics final
		{
			uint32 draw_calls, quad_count;

			ML_NODISCARD auto get_vertex_count() const { return quad_count * 4u; }

			ML_NODISCARD auto get_index_count() const { return quad_count * 6u; }
		};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static bool initialize();

		static void finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static metrics const & get_metrics() noexcept;

		static void reset_metrics() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void begin_scene(camera const & cam);

		static void end_scene();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void draw_quad(float32 x, float32 y, float32 w, float32 h, float32 rotation = 0.f);

		static void draw_quad(vec2 const & pos, vec2 const & size, float32 rotation = 0.f);

		static void draw_quad(float_rect const & bounds, float32 rotation = 0.f);

		static void draw_quad(mat4 const & m);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDERER_2D_HPP_