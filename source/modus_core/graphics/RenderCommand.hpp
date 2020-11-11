#ifndef _ML_RENDER_COMMAND_HPP_
#define _ML_RENDER_COMMAND_HPP_

#include <modus_core/graphics/RenderAPI.hpp>

// COMMAND
namespace ml::gfx
{
	// render command
	class command final : public ds::method< void(render_context *) >
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

		ML_NODISCARD static auto clear(uint32_t mask) noexcept
		{
			return command{ &render_context::clear, mask };
		}

		ML_NODISCARD static auto draw(ds::ref<vertexarray> const & value) noexcept
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
		> ML_NODISCARD static auto bind_texture(Value && value, uint32_t slot = 0) noexcept
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

// EXECUTE
namespace ml::gfx
{
	// execute render commands
	template <class Ctx, class Arg0, class ... Args
	> static void execute(Ctx && ctx, Arg0 && arg0, Args && ... args) noexcept
	{
		// do execute
		auto do_execute = [](auto && cmd, auto && ctx) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(ctx)>>)
			{
				std::invoke(ML_forward(cmd), ctx);
			}
			else
			{
				std::invoke(ML_forward(cmd), ctx.get());
			}
		};

		do_execute(ML_forward(arg0), ML_forward(ctx)); // arg0

		if constexpr (0 < sizeof...(args))
		{
			meta::for_args([&](auto && cmd) noexcept
			{
				do_execute(ML_forward(cmd), ML_forward(ctx)); // args...
			}
			, ML_forward(args)...);
		}

		//if constexpr (0 == sizeof...(Args))
		//{
		//	if constexpr (std::is_convertible_v<std::decay_t<decltype(arg0)>, gfx::command>)
		//	{
		//		if constexpr (std::is_scalar_v<std::decay_t<decltype(ctx)>>)
		//		{
		//			std::invoke(ML_forward(arg0), ctx);
		//		}
		//		else
		//		{
		//			std::invoke(ML_forward(arg0), ctx.get());
		//		}
		//	}
		//	else
		//	{
		//		for (auto const & cmd : ML_forward(arg0))
		//		{
		//			_ML_GFX execute(ML_forward(ctx), cmd);
		//		}
		//	}
		//}
		//else
		//{
		//	_ML_GFX execute(ML_forward(ctx), ML_forward(arg0));
		//
		//	meta::for_args([&](auto && cmd) noexcept
		//	{
		//		_ML_GFX execute(ML_forward(ctx), ML_forward(cmd));
		//	}
		//	, ML_forward(args)...);
		//}
	}
}

#endif // !_ML_RENDER_COMMAND_HPP_