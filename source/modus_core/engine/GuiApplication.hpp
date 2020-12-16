#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/engine/Performance.hpp>
#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/window/NativeWindow.hpp>
#include <modus_core/graphics/RenderCommand.hpp>
#include <modus_core/graphics/Viewport.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// gui application
	struct ML_CORE_API gui_application : core_application, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		gui_application(int32 argc, char * argv[], json const & attribs = {}, allocator_type alloc = {});
		
		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() override
		{
			m_loop.process();

			return core_application::exec();
		}

		virtual void exit(int32 exit_code) override
		{
			m_loop.halt();

			return core_application::exit(exit_code);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_fps() const noexcept -> fps_tracker *
		{
			return const_cast<fps_tracker *>(&m_fps_tracker);
		}

		ML_NODISCARD auto get_input() const noexcept -> input_state *
		{
			return const_cast<input_state *>(&m_input_state);
		}

		ML_NODISCARD auto get_loop() const noexcept -> loop_system *
		{
			return const_cast<loop_system *>(&m_loop);
		}

		ML_NODISCARD auto get_window() const noexcept -> native_window *
		{
			return const_cast<native_window *>(&m_window);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_render_device() const noexcept -> gfx::render_device *
		{
			return m_render_device.get();
		}

		ML_NODISCARD auto get_render_context() const noexcept -> ds::ref<gfx::render_context> const &
		{
			return m_render_device->get_active_context();
		}

		void set_render_device(gfx::render_device * value) noexcept
		{
			m_render_device.reset(value);
		}

		void set_render_context(ds::ref<gfx::render_context> const & value) noexcept
		{
			m_render_device->set_active_context(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui_context() const noexcept -> ImGuiContext *
		{
			return m_imgui_context.get();
		}

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace *
		{
			return const_cast<ImGuiExt::Dockspace *>(&m_dockspace);
		}

		ML_NODISCARD auto get_main_menu() const noexcept -> ImGuiExt::MainMenuBar *
		{
			return const_cast<ImGuiExt::MainMenuBar *>(&m_main_menu);
		}

		void set_imgui_context(ImGuiContext * value) noexcept
		{
			m_imgui_context.reset(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus						m_dispatcher	; // event bus
		loop_system						m_loop			; // main loop
		native_window					m_window		; // main window
		ds::scary<gfx::render_device>	m_render_device	; // render device
		ds::scary<ImGuiContext>			m_imgui_context	; // imgui context
		ImGuiExt::Dockspace				m_dockspace		; // main dockspace
		ImGuiExt::MainMenuBar			m_main_menu		; // main menubar
		fps_tracker						m_fps_tracker	; // fps tracker
		input_state						m_input_state	; // input state

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global gui_application
namespace ml::globals
{
	ML_decl_global(gui_application) get() noexcept;

	ML_decl_global(gui_application) set(gui_application * value) noexcept;
}

#endif // !_ML_GUI_APPLICATION_HPP_