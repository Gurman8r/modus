#ifndef _ML_RENDER_COMMAND_HPP_
#define _ML_RENDER_COMMAND_HPP_

#include <modus_core/graphics/RenderAPI.hpp>

// EXECUTE
namespace ml::gfx
{
	// execute commands
	template <class Context, class Arg0, class ... Args
	> static void execute(Context && ctx, Arg0 && arg0, Args && ... args) noexcept
	{
		ML_assert(ctx);

		ctx->execute(ML_forward(arg0), ML_forward(args)...);
	}
}

// COMMAND
namespace ml::gfx
{
	// render command
	struct command : public std::function< void(render_context *) >
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using std::function< void(render_context *) >::function;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static command set_alpha_state(alpha_state const & value) noexcept
		{
			return std::bind(&render_context::set_alpha_state, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_blend_state(blend_state const & value) noexcept
		{
			return std::bind(&render_context::set_blend_state, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_clear_color(color const & value) noexcept
		{
			return std::bind(&render_context::set_clear_color, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_cull_state(cull_state const & value) noexcept
		{
			return std::bind(&render_context::set_cull_state, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_depth_state(depth_state const & value) noexcept
		{
			return std::bind(&render_context::set_depth_state, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_stencil_state(stencil_state const & value) noexcept
		{
			return std::bind(&render_context::set_stencil_state, std::placeholders::_1, value);
		}

		ML_NODISCARD static command set_viewport(int_rect const & value) noexcept
		{
			return std::bind(&render_context::set_viewport, std::placeholders::_1, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static command clear(uint32 value) noexcept
		{
			return std::bind(&render_context::clear, std::placeholders::_1, value);
		}

		ML_NODISCARD static command draw(ds::ref<vertexarray> const & value) noexcept
		{
			return std::bind(&render_context::draw, std::placeholders::_1, value);
		}

		ML_NODISCARD static command draw_arrays(uint32 mode, uint32 first, size_t count) noexcept
		{
			return std::bind(&render_context::draw_arrays, std::placeholders::_1, mode, first, count);
		}

		ML_NODISCARD static command draw_indexed(uint32 mode, size_t count) noexcept
		{
			return std::bind(&render_context::draw_indexed, std::placeholders::_1, mode, count);
		}

		ML_NODISCARD static command flush() noexcept
		{
			return std::bind(&render_context::flush, std::placeholders::_1);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value
		> ML_NODISCARD static command bind_vertexarray(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_vertexarray, std::placeholders::_1, (vertexarray *)value);
			}
			else
			{
				return std::bind(&render_context::bind_vertexarray, std::placeholders::_1, (vertexarray *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_vertexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_vertexbuffer, std::placeholders::_1, (vertexbuffer *)value);
			}
			else
			{
				return std::bind(&render_context::bind_vertexbuffer, std::placeholders::_1, (vertexbuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_indexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_indexbuffer, std::placeholders::_1, (indexbuffer *)value);
			}
			else
			{
				return std::bind(&render_context::bind_indexbuffer, std::placeholders::_1, (indexbuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_texture(Value && value, uint32 slot = 0) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_texture, std::placeholders::_1, (texture *)value);
			}
			else
			{
				return std::bind(&render_context::bind_texture, std::placeholders::_1, (texture *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_framebuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_framebuffer, std::placeholders::_1, (framebuffer *)value);
			}
			else
			{
				return std::bind(&render_context::bind_framebuffer, std::placeholders::_1, (framebuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_program(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_program, std::placeholders::_1, (program *)value);
			}
			else
			{
				return std::bind(&render_context::bind_program, std::placeholders::_1, (program *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static command bind_shader(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return std::bind(&render_context::bind_shader, std::placeholders::_1, (shader *)value);
			}
			else
			{
				return std::bind(&render_context::bind_shader, std::placeholders::_1, (shader *)value.get());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD static command upload(uniform_id loc, T value) noexcept
		{
			return std::bind(static_cast<void(render_context:: *)(uniform_id, T)
			>(&render_context::upload), std::placeholders::_1, loc, value);
		}

		template <class T
		> ML_NODISCARD static command upload(uniform_id loc, T const & value) noexcept
		{
			return std::bind(static_cast<void(render_context:: *)(uniform_id, T const &)
			>(&render_context::upload), std::placeholders::_1, loc, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}



#endif // !_ML_RENDER_COMMAND_HPP_