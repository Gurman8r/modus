#ifndef _ML_MAIN_WINDOW_HPP_
#define _ML_MAIN_WINDOW_HPP_

#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// main window
	struct ML_CORE_API main_window : render_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using render_window::allocator_type;

		using render_window::render_window;

		main_window(allocator_type alloc = {}) noexcept;

		virtual ~main_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string const &			title,
			video_mode const &			vm		= {},
			context_settings const &	cs		= {},
			window_hints_				hints	= window_hints_default
		) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui() const noexcept -> ds::scary<ImGuiContext> const &
		{
			return m_imgui;
		}

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace *
		{
			return const_cast<ImGuiExt::Dockspace *>(&m_dockspace);
		}

		ML_NODISCARD auto get_menubar() const noexcept -> ImGuiExt::MainMenuBar *
		{
			return const_cast<ImGuiExt::MainMenuBar *>(&m_menubar);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool initialize_imgui(bool install_callbacks = true);
		
		void finalize_imgui();

		void new_frame();

		void render_frame();

		template <class Fn, class ... Args
		> void do_frame(Fn && fn, Args && ... args) noexcept
		{
			this->new_frame();

			std::invoke(ML_forward(fn), this, ML_forward(args)...);

			this->render_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool load_style(fs::path const & path);

		bool load_style(json const & j); // NYI

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD std::optional<fs::path> get_open_file_name(cstring filter = "") const;

		ML_NODISCARD std::optional<fs::path> get_save_file_name(cstring filter = "") const;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<ImGuiContext>	m_imgui		; // imgui
		ImGuiExt::Dockspace		m_dockspace	; // dockspace
		ImGuiExt::MainMenuBar	m_menubar	; // menubar

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MAIN_WINDOW_HPP_