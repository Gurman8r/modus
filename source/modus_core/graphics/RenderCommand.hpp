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

		template <class Fn, class ... Args
		> ML_NODISCARD static command bind(Fn && fn, Args && ... args) noexcept
		{
			return std::bind(ML_forward(fn), std::placeholders::_1, ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto set_alpha_state(alpha_state const & value) noexcept
		{
			return command::bind(&render_context::set_alpha_state, value);
		}

		ML_NODISCARD static auto set_blend_state(blend_state const & value) noexcept
		{
			return command::bind(&render_context::set_blend_state, value);
		}

		ML_NODISCARD static auto set_clear_color(color const & value) noexcept
		{
			return command::bind(&render_context::set_clear_color, value);
		}

		ML_NODISCARD static auto set_cull_state(cull_state const & value) noexcept
		{
			return command::bind(&render_context::set_cull_state, value);
		}

		ML_NODISCARD static auto set_depth_state(depth_state const & value) noexcept
		{
			return command::bind(&render_context::set_depth_state, value);
		}

		ML_NODISCARD static auto set_stencil_state(stencil_state const & value) noexcept
		{
			return command::bind(&render_context::set_stencil_state, value);
		}

		ML_NODISCARD static auto set_viewport(int_rect const & value) noexcept
		{
			return command::bind(&render_context::set_viewport, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto clear(uint32 mask) noexcept
		{
			return command::bind(&render_context::clear, mask);
		}

		ML_NODISCARD static auto draw(ds::ref<vertexarray> const & value) noexcept
		{
			return command::bind(&render_context::draw, value);
		}

		ML_NODISCARD static auto draw_arrays(uint32 mode, uint32 first, size_t count) noexcept
		{
			return command::bind(&render_context::draw_arrays, mode, first, count);
		}

		ML_NODISCARD static auto draw_indexed(uint32 mode, size_t count) noexcept
		{
			return command::bind(&render_context::draw_indexed, mode, count);
		}

		ML_NODISCARD static auto flush() noexcept
		{
			return command::bind(&render_context::flush);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Value
		> ML_NODISCARD static auto bind_vertexarray(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_vertexarray, (vertexarray *)value);
			}
			else
			{
				return command::bind(&render_context::bind_vertexarray, (vertexarray *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_vertexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_vertexbuffer, (vertexbuffer *)value);
			}
			else
			{
				return command::bind(&render_context::bind_vertexbuffer, (vertexbuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_indexbuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_indexbuffer, (indexbuffer *)value);
			}
			else
			{
				return command::bind(&render_context::bind_indexbuffer, (indexbuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_texture(Value && value, uint32 slot = 0) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_texture, (texture *)value);
			}
			else
			{
				return command::bind(&render_context::bind_texture, (texture *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_framebuffer(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_framebuffer, (framebuffer *)value);
			}
			else
			{
				return command::bind(&render_context::bind_framebuffer, (framebuffer *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_program(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_program, (program *)value);
			}
			else
			{
				return command::bind(&render_context::bind_program, (program *)value.get());
			}
		}

		template <class Value
		> ML_NODISCARD static auto bind_shader(Value && value) noexcept
		{
			if constexpr (std::is_scalar_v<std::decay_t<decltype(value)>>)
			{
				return command::bind(&render_context::bind_shader, (shader *)value);
			}
			else
			{
				return command::bind(&render_context::bind_shader, (shader *)value.get());
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class T
		> ML_NODISCARD static auto upload(uniform_id loc, T value) noexcept
		{
			return command::bind(static_cast<void(render_context:: *)(uniform_id, T)
			>(&render_context::upload), loc, value);
		}

		template <class T
		> ML_NODISCARD static auto upload(uniform_id loc, T const & value) noexcept
		{
			return command::bind(static_cast<void(render_context:: *)(uniform_id, T const &)
			>(&render_context::upload), loc, value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// DRAW LIST
namespace ml::gfx
{
	// draw list
	struct draw_list : command
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		template <class ... Args
		> draw_list(std::allocator_arg_t, allocator_type alloc, Args && ... args) noexcept
			: command{ [&](auto ctx) noexcept
			{
				for (ds::scope<command> const & cmd : m_commands)
				{
					ctx->execute(*cmd);
				}
			} }
			, m_commands{ alloc }
		{
			m_commands.reserve(sizeof...(args));

			meta::for_args([&](auto && cmd) noexcept
			{
				this->add_command(ML_forward(cmd));
			}
			, ML_forward(args)...);
		}

		template <class ... Args
		> draw_list(Args && ... args) noexcept
			: draw_list{ std::allocator_arg, {}, ML_forward(args)... }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void add_command(command const & value)
		{
			if (value && this != std::addressof(value))
			{
				this->new_command(value);
			}
		}

		void add_command(command && value) noexcept
		{
			if (value && this != std::addressof(value))
			{
				this->new_command(value);
			}
		}

		template <class Cmd = command, class ... Args
		> void new_command(Args && ... args) noexcept
		{
			m_commands.emplace_back(new Cmd{ ML_forward(args)... });
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		draw_list & operator+=(command const & value)
		{
			this->add_command(value);
			return (*this);
		}

		draw_list & operator+=(command && value) noexcept
		{
			this->add_command(std::move(value));
			return (*this);
		}

		draw_list & operator+=(std::initializer_list<command> value)
		{
			for (command const & e : value) { this->add_command(e); }
			return (*this);
		}

		draw_list & operator+=(ds::list<command> const & value)
		{
			for (command const & e : value) { this->add_command(e); }
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator*() & noexcept -> ds::list<ds::scope<command>> &
		{
			return m_commands;
		}

		ML_NODISCARD auto operator*() const & noexcept -> ds::list<ds::scope<command>> const &
		{
			return m_commands;
		}

		ML_NODISCARD auto operator->() noexcept -> ds::list<ds::scope<command>> *
		{
			return &m_commands;
		}

		ML_NODISCARD auto operator->() const noexcept -> ds::list<ds::scope<command>> const *
		{
			return &m_commands;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::list<ds::scope<command>> m_commands;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDER_COMMAND_HPP_