#ifndef _ML_RENDER_COMMAND_HPP_
#define _ML_RENDER_COMMAND_HPP_

#include <modus_core/graphics/RenderAPI.hpp>

// EXECUTE
namespace ml::gfx
{
	// execute commands
	template <class Ctx, class Arg0, class ... Args
	> static void execute(Ctx && ctx, Arg0 && arg0, Args && ... args) noexcept
	{
		ML_assert(ctx);

		ctx->execute(ML_forward(arg0), ML_forward(args)...);
	}
}

// COMMAND
namespace ml::gfx
{
	// render command
	class command : public ds::method< void(render_context *) >
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> command(Fn && fn, Args && ... args) noexcept
			: method{ std::bind(ML_forward(fn), std::placeholders::_1, ML_forward(args)...) }
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

		ML_NODISCARD static auto set_viewport(int_rect const & value) noexcept
		{
			return command{ &render_context::set_viewport, value };
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto clear(uint32 mask) noexcept
		{
			return command{ &render_context::clear, mask };
		}

		ML_NODISCARD static auto draw(ds::ref<vertexarray> const & value) noexcept
		{
			return command{ &render_context::draw, value };
		}

		ML_NODISCARD static auto draw_arrays(uint32 mode, uint32 first, size_t count) noexcept
		{
			return command{ &render_context::draw_arrays, mode, first, count };
		}

		ML_NODISCARD static auto draw_indexed(uint32 mode, size_t count) noexcept
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
				return command{ &render_context::bind_vertexarray, value };
			}
			else
			{
				return command{ &render_context::bind_vertexarray, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_vertexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_vertexbuffer, value };
			}
			else
			{
				return command{ &render_context::bind_vertexbuffer, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_indexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_indexbuffer, value };
			}
			else
			{
				return command{ &render_context::bind_indexbuffer, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_texture(Value && value, uint32 slot = 0) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_texture, value };
			}
			else
			{
				return command{ &render_context::bind_texture, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_framebuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_framebuffer, value };
			}
			else
			{
				return command{ &render_context::bind_framebuffer, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_program(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_program, value };
			}
			else
			{
				return command{ &render_context::bind_program, value.get() };
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_shader(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command{ &render_context::bind_shader, value };
			}
			else
			{
				return command{ &render_context::bind_shader, value.get() };
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

// TECHNIQUE
namespace ml::gfx
{
	// technique
	class technique : public command
	{
	public:
		template <class Arg0, class ... Args
		> technique(Arg0 && arg0, Args && ... args) noexcept : command{ std::bind
		(
			&gfx::execute, std::placeholders::_1, ML_forward(arg0), ML_forward(args)...
		)}
		{
		}
	};
}

#endif // !_ML_RENDER_COMMAND_HPP_