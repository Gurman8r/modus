#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

// WIP

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/imgui/ImGuiEvents.hpp>
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

		ML_NODISCARD auto get_delta_time() const noexcept -> duration { return m_delta_time; }
		
		ML_NODISCARD auto get_frame_count() const noexcept -> uint64_t { return m_frame_count; }
		
		ML_NODISCARD auto get_frame_rate() const noexcept -> float_t { return m_frame_rate; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
		
		ML_NODISCARD auto get_cursor_pos() const noexcept -> vec2d const & { return m_cursor_pos; }
		
		ML_NODISCARD auto get_mouse() const noexcept -> mouse_state const & { return m_mouse; }
		
		ML_NODISCARD auto get_mouse(size_t i) const noexcept -> int32_t { return m_mouse[i]; }
		
		ML_NODISCARD auto get_keyboard() const noexcept -> keyboard_state const & { return m_keyboard; }
		
		ML_NODISCARD auto get_keyboard(size_t i) const noexcept -> int32_t { return m_keyboard[i]; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace * { return m_docker.get(); }

		ML_NODISCARD auto get_imgui() const noexcept -> ImGuiContext * { return m_imgui.get(); }

		ML_NODISCARD auto get_window() const noexcept -> render_window * { return m_window.get(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::get_bus;

		using core_application::fire_event;

		using core_application::post_event;

		using core_application::process_events;

	protected:
		using core_application::subscribe;

		using core_application::unsubscribe;

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		timer							m_loop_timer	; // 
		duration						m_delta_time	; // 
		uint64_t						m_frame_count	; // 
		float_t							m_frame_rate	; // 
		float_t							m_fps_accum		; // 
		size_t							m_fps_index		; // 
		ds::array<float_t, 120>			m_fps_times		; // 

		vec2d							m_cursor_pos	; // 
		mouse_state						m_mouse			; // 
		keyboard_state					m_keyboard		; // 

		ds::scope<render_window>		m_window		; // 
		ds::raw<ImGuiContext>		m_imgui			; // 
		ds::scope<ImGuiExt::Dockspace>	m_docker		; // 

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