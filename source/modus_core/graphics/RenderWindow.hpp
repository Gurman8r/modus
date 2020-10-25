#ifndef _ML_RENDER_WINDOW_HPP_
#define _ML_RENDER_WINDOW_HPP_

#include <modus_core/window/NativeWindow.hpp>
#include <modus_core/graphics/RenderCommand.hpp>

namespace ml
{
	struct ML_CORE_API render_window : native_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = typename render_window;

		render_window(allocator_type alloc = {}) noexcept;

		render_window(
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			allocator_type				alloc	= {},
			void *						userptr	= nullptr) noexcept;

		virtual ~render_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		virtual bool open(
			ds::string			const &	title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			void *						userptr	= nullptr) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Arg0, class ... Args
		> void execute(Arg0 && arg0, Args && ... args) const noexcept
		{
			gfx::execute(m_ctx, ML_forward(arg0), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_render_device() const & noexcept -> manual<gfx::render_device> const & { return m_dev; }

		ML_NODISCARD auto get_render_context() const & noexcept -> shared<gfx::render_context> const & { return m_ctx; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		manual<	gfx::render_device	> m_dev; // device
		shared<	gfx::render_context	> m_ctx; // context

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDER_WINDOW_HPP_