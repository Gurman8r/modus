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

		ML_NODISCARD auto get_main_window() const noexcept -> main_window *
		{
			return m_window.get();
		}

		ML_NODISCARD auto get_window_manager() const noexcept -> window_manager const *
		{
			return m_window->get_window_manager();
		}

		ML_NODISCARD auto get_render_device() const -> gfx::render_device *
		{
			return m_window->get_render_device();
		}

		ML_NODISCARD auto get_render_context() const -> ds::ref<gfx::render_context> const &
		{
			return m_window->get_render_context();
		}

		ML_NODISCARD auto get_imgui_context() const noexcept -> ImGuiContext *
		{
			return m_window->get_imgui_context();
		}

		ML_NODISCARD auto get_dockspace() const noexcept -> ImGuiExt::Dockspace *
		{
			return m_window->get_dockspace();
		}

		ML_NODISCARD auto get_menubar() const noexcept -> ImGuiExt::MenuBar *
		{
			return m_window->get_menubar();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_delta_time() const noexcept -> duration { return m_delta_time; }
		
		ML_NODISCARD auto get_frame_count() const noexcept -> uint64_t { return m_frame_count; }
		
		ML_NODISCARD auto get_frame_rate() const noexcept -> float_t { return m_frame_rate; }

		ML_NODISCARD auto get_cursor_pos() const noexcept -> vec2d const & { return m_cursor_pos; }
		
		ML_NODISCARD auto get_mouse() const noexcept -> mouse_state const & { return m_mouse; }
		
		ML_NODISCARD auto get_keyboard() const noexcept -> keyboard_state const & { return m_keyboard; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_object::get_bus;

	protected:
		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scope<main_window>	m_window		; // 
		timer					m_loop_timer	; // 
		duration				m_delta_time	; // 
		uint64_t				m_frame_count	; // 
		float_t					m_frame_rate	; // 
		float_t					m_fps_accum		; // 
		size_t					m_fps_index		; // 
		ds::array<float_t, 120>	m_fps_times		; // 
		vec2d					m_cursor_pos	; // 
		mouse_state				m_mouse			; // 
		keyboard_state			m_keyboard		; // 

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