#ifndef _ML_RENDER_WINDOW_HPP_
#define _ML_RENDER_WINDOW_HPP_

#include <core/window/Window.hpp>
#include <core/graphics/RenderAPI.hpp>

namespace ml
{
	struct ML_CORE_API render_window : window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		render_window(allocator_type alloc = {}) noexcept;

		render_window(window_settings const & settings, allocator_type alloc = {}) noexcept;

		virtual ~render_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual bool open(window_settings const & settings) override;

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