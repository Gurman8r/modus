#ifndef _ML_MAIN_WINDOW_HPP_
#define _ML_MAIN_WINDOW_HPP_

#include <modus_core/engine/Object.hpp>
#include <modus_core/engine/PlatformAPI.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>

namespace ml
{
	// main window
	struct ML_CORE_API main_window : core_object, render_window
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
			void *						userptr = nullptr,
			allocator_type				alloc	= {}) noexcept;

		virtual ~main_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual bool open(
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			void *						userptr	= nullptr) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool initialize_imgui(bool install_callbacks = true);

		void finalize_imgui();

		bool load_imgui_style(fs::path const & path);

		void begin_imgui_frame();

		void end_imgui_frame();

		template <class ... Args
		> void do_imgui_frame(Args && ... args) noexcept
		{
			this->begin_imgui_frame();

			if constexpr (0 < sizeof...(Args)) { std::invoke(ML_forward(args)...); }

			this->end_imgui_frame();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui() const noexcept -> ds::scary<ImGuiContext> const &
		{
			return m_imgui;
		}

		ML_NODISCARD auto get_menubar() const noexcept -> ImGuiExt::MenuBar *
		{
			return const_cast<ImGuiExt::MenuBar *>(&m_menubar);
		}

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace *
		{
			return const_cast<ImGuiExt::Dockspace *>(&m_dockspace);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD std::optional<fs::path> get_open_file_name(cstring filter = "") const
		{
			return platform_api::get_open_file_name(get_native_handle(), filter);
		}

		ML_NODISCARD std::optional<fs::path> get_save_file_name(cstring filter = "") const
		{
			return platform_api::get_save_file_name(get_native_handle(), filter);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_object::get_bus;

	protected:
		using core_object::subscribe;

		using core_object::unsubscribe;

		virtual void on_event(event const & value) override {}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<ImGuiContext>	m_imgui		; // imgui
		ImGuiExt::MenuBar		m_menubar	; // menubar
		ImGuiExt::Dockspace		m_dockspace	; // dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MAIN_WINDOW_HPP_