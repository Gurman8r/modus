#ifndef _ML_MAIN_WINDOW_HPP_
#define _ML_MAIN_WINDOW_HPP_

#include <modus_core/engine/Object.hpp>
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

		bool startup_imgui(bool install_callbacks = true) noexcept
		{
			return _ML ImGui_Startup(this, install_callbacks);
		}

		void shutdown_imgui() noexcept { _ML ImGui_Shutdown(this, get_imgui()); }

		void begin_imgui_frame() noexcept { _ML ImGui_NewFrame(this, get_imgui()); }

		void end_imgui_frame() noexcept { _ML ImGui_RenderFrame(this, get_imgui()); }

		template <class Fn, class ... Args
		> void do_imgui_frame(Fn && fn, Args && ... args) noexcept
		{
			_ML ImGui_DoFrame(this, get_imgui(), [&]() noexcept
			{
				ImGuiExt_ScopeID(this);

				std::invoke(ML_forward(fn), ML_forward(args)...);
			});
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_docker() const noexcept -> ImGuiExt::Dockspace * { return m_docker.get(); }

		ML_NODISCARD auto get_imgui() const noexcept -> ImGuiContext * { return m_imgui.get(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_object::get_bus;

	protected:
		using core_object::subscribe;

		using core_object::unsubscribe;

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<ImGuiContext>			m_imgui		; // imgui context
		ds::scope<ImGuiExt::Dockspace>	m_docker	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global main_window
namespace ml::globals
{

}

#endif // !1