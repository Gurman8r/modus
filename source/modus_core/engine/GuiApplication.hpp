#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/engine/Performance.hpp>
#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/detail/LoopSystem.hpp>

namespace ml
{
	// gui application
	struct ML_CORE_API gui_application : core_application, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		explicit gui_application(int32 argc, char * argv[], allocator_type alloc = {});

		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() override
		{
			m_main_loop.process();

			return core_application::exec();
		}

		virtual void exit(int32 exit_code) override
		{
			m_main_loop.halt();

			core_application::exit(exit_code);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_fps() const noexcept -> fps_tracker<> const *
		{
			return &m_fps_tracker;
		}

		ML_NODISCARD auto get_main_loop() const noexcept -> loop_system *
		{
			return const_cast<loop_system *>(&m_main_loop);
		}

		ML_NODISCARD auto get_main_window() const noexcept -> main_window *
		{
			return const_cast<main_window *>(&m_main_window);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD std::optional<fs::path> open_file_name(ds::string const & filter = "") const;

		ML_NODISCARD std::optional<fs::path> save_file_name(ds::string const & filter = "") const;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus		m_dispatcher	; // event bus
		loop_system		m_main_loop		; // main loop
		main_window		m_main_window	; // main window
		fps_tracker<>	m_fps_tracker	; // fps tracker

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