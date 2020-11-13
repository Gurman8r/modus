#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

// WIP

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/MainWindow.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/window/WindowEvents.hpp>

namespace ml
{
	struct ML_CORE_API gui_application : core_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		explicit gui_application(int32_t argc, char * argv[], allocator_type alloc = {});

		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_window() const noexcept -> main_window *
		{
			return const_cast<main_window *>(&m_window);
		}

		ML_NODISCARD auto get_delta_time() const noexcept -> duration { return m_delta_time; }

		ML_NODISCARD auto get_frame_count() const noexcept -> uint64_t { return m_frame_count; }

		ML_NODISCARD auto get_frame_rate() const noexcept -> float_t { return m_frame_rate; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		main_window				m_window		; // 
		timer					m_loop_timer	; // 
		duration				m_delta_time	; // 
		uint64_t				m_frame_count	; // 
		float_t					m_frame_rate	; // 
		float_t					m_fps_accum		; // 
		size_t					m_fps_index		; // 
		ds::array<float_t, 120>	m_fps_times		; // 

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