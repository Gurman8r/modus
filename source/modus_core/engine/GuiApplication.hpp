#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/detail/EventSystem.hpp>
#include <modus_core/detail/LoopSystem.hpp>
#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/engine/Performance.hpp>

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

	public:
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

		ML_NODISCARD auto get_window() const noexcept -> main_window *
		{
			return const_cast<main_window *>(&m_window);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_enter();

		virtual void on_exit();

		virtual void on_idle(duration const & dt);

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus	m_dispatcher	; // event bus
		loop_system	m_loop			; // main loop
		main_window	m_window		; // main window
		fps_tracker	m_fps_tracker	; // fps tracker
		input_state	m_input_state	; // input state

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