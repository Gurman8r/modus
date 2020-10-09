#ifndef _ML_RENDER_COMMAND_HPP_
#define _ML_RENDER_COMMAND_HPP_

#include <graphics/RenderAPI.hpp>

namespace ml::gfx
{
	// command executor helper
	template <class Cmd, class Ctx
	> static auto execute(Cmd && cmd, Ctx && ctx) noexcept
	{
		if constexpr (std::is_scalar_v<std::decay_t<decltype(ctx)>>)
		{
			return std::invoke(ML_forward(cmd), ML_forward(ctx));
		}
		else
		{
			return execute(ML_forward(cmd), ML_forward(ctx).get());
		}
	}

	// render command
	class command final : public std::function< void(render_context *) >
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> command(Fn && fn, Args && ... args) noexcept
			: function{ std::bind(ML_forward(fn), std::placeholders::_1, ML_forward(args)...) }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto set_alpha_state(alpha_state const & value) noexcept
		{
			return command{ &render_context::set_alpha_state, value };
		}

		ML_NODISCARD static auto set_blend_state(blend_state const & value) noexcept
		{
			return command{ &render_context::set_blend_state, value };
		}

		ML_NODISCARD static auto set_clear_color(color const & value) noexcept
		{
			return command{ &render_context::set_clear_color, value };
		}

		ML_NODISCARD static auto set_cull_state(cull_state const & value) noexcept
		{
			return command{ &render_context::set_cull_state, value };
		}

		ML_NODISCARD static auto set_depth_state(depth_state const & value) noexcept
		{
			return command{ &render_context::set_depth_state, value };
		}

		ML_NODISCARD static auto set_stencil_state(stencil_state const & value) noexcept
		{
			return command{ &render_context::set_stencil_state, value };
		}

		ML_NODISCARD static auto set_viewport(int_rect const & bounds) noexcept
		{
			return command{ &render_context::set_viewport, bounds };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto clear(uint32_t mask) noexcept
		{
			return command{ &render_context::clear, mask };
		}

		ML_NODISCARD static auto draw(shared<vertexarray> const & value) noexcept
		{
			return command{ &render_context::draw, value };
		}

		ML_NODISCARD static auto draw_arrays(uint32_t mode, uint32_t first, size_t count) noexcept
		{
			return command{ &render_context::draw_arrays, mode, first, count };
		}

		ML_NODISCARD static auto draw_indexed(uint32_t mode, size_t count) noexcept
		{
			return command{ &render_context::draw_indexed, mode, count };
		}

		ML_NODISCARD static auto flush() noexcept
		{
			return command{ &render_context::flush };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value
		> ML_NODISCARD static auto bind_vertexarray(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_vertexarray, ML_forward(value) };
			}
			else
			{
				return bind_vertexarray(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_vertexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_vertexbuffer, ML_forward(value) };
			}
			else
			{
				return bind_vertexbuffer(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_indexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_indexbuffer, ML_forward(value) };
			}
			else
			{
				return bind_indexbuffer(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_texture(Value && value, uint32_t slot = 0) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_texture, ML_forward(value) };
			}
			else
			{
				return bind_texture(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_framebuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_framebuffer, ML_forward(value) };
			}
			else
			{
				return bind_framebuffer(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_program(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_program, ML_forward(value) };
			}
			else
			{
				return bind_program(ML_forward(value).get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_shader(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_shader, ML_forward(value) };
			}
			else
			{
				return bind_shader(ML_forward(value).get());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD static auto upload(uniform_id loc, T value) noexcept
		{
			return command{ static_cast<void(render_context::*)(uniform_id, T)
			>(&render_context::upload), loc, value };
		}

		template <class T
		> ML_NODISCARD static auto upload(uniform_id loc, T const & value) noexcept
		{
			return command{ static_cast<void(render_context::*)(uniform_id, T const &)
			>(&render_context::upload), loc, value };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDER_COMMAND_HPP_