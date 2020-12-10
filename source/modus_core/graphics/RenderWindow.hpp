#ifndef _ML_RENDER_WINDOW_HPP_
#define _ML_RENDER_WINDOW_HPP_

#include <modus_core/window/NativeWindow.hpp>
#include <modus_core/graphics/Canvas.hpp>

namespace ml
{
	// render window
	struct ML_CORE_API render_window : native_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using native_window::allocator_type;

		using native_window::native_window;

		render_window(allocator_type alloc = {}) noexcept;

		virtual ~render_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD virtual bool open(
			ds::string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_render_device() const noexcept -> ds::scary<gfx::render_device> const &
		{
			return m_device;
		}

		ML_NODISCARD auto get_render_context() const noexcept -> ds::ref<gfx::render_context> const &
		{
			return ML_check(m_device)->get_active_context();
		}

		auto set_render_context(ds::ref<gfx::render_context> const & value) noexcept -> ds::ref<gfx::render_context> &
		{
			return ML_check(m_device)->set_active_context(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<gfx::render_device> m_device; // render device

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RENDER_WINDOW_HPP_