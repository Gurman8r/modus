#ifndef _ML_MAIN_WINDOW_HPP_
#define _ML_MAIN_WINDOW_HPP_

#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

namespace ml
{
	// main window
	struct ML_CORE_API main_window : render_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using self_type = main_window;

		using base_type = render_window;

		using base_type::allocator_type;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		main_window(allocator_type alloc = {}) noexcept;

		main_window(
			ds::string			const & title,
			video_mode			const & vm = {},
			context_settings	const & cs = {},
			window_hints_				hints = window_hints_default,
			void *						userptr = nullptr,
			allocator_type				alloc = {}) noexcept;

		virtual ~main_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual bool open(
			ds::string			const & title,
			video_mode			const & vm = {},
			context_settings	const & cs = {},
			window_hints_				hints = window_hints_default,
			void *						userptr = nullptr) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_docker() const noexcept -> ImGuiExt::Dockspace * { return m_docker.get(); }

		ML_NODISCARD auto get_imgui() const noexcept -> ImGuiContext * { return m_imgui.get(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<ImGuiContext>			m_imgui		; // imgui context
		ds::scope<ImGuiExt::Dockspace>	m_docker	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !1