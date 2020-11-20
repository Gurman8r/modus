#ifndef _ML_MAIN_WINDOW_HPP_
#define _ML_MAIN_WINDOW_HPP_

#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// main window
	struct ML_CORE_API main_window : event_listener, render_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using render_window::allocator_type;

		main_window(event_bus * bus, allocator_type alloc = {}) noexcept;

		main_window(
			event_bus *					bus,
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			allocator_type				alloc	= {}) noexcept;

		virtual ~main_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_imgui_style(fs::path const & path);

		void begin_imgui_frame();

		void end_imgui_frame();

		template <class Fn, class ... Args
		> void do_imgui_frame(Fn && fn, Args && ... args) noexcept
		{
			this->begin_imgui_frame();

			std::invoke(ML_forward(fn), this, ML_forward(args)...);

			this->end_imgui_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui() const noexcept -> ds::scary<ImGuiContext> const &
		{
			return m_imgui;
		}

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace *
		{
			return const_cast<ImGuiExt::Dockspace *>(&m_dockspace);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using event_listener::get_bus;

	protected:
		using event_listener::subscribe;

		using event_listener::unsubscribe;

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<ImGuiContext>	m_imgui		; // imgui
		ImGuiExt::Dockspace		m_dockspace	; // dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MAIN_WINDOW_HPP_