#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/runtime/CoreApplication.hpp>
#include <modus_core/graphics/RenderTarget.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/scene/Scene.hpp>
#include <modus_core/window/NativeWindow.hpp>

namespace ml
{
	// gui application
	struct ML_CORE_API gui_application : core_application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using core_application::allocator_type;

		gui_application(int32 argc, char * argv[], json const & argj = {}, allocator_type alloc = {});
		
		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		virtual int32 exec() override
		{
			m_loop.loop();
			return core_application::exec();
		}

		virtual void exit(int32 exit_code) override
		{
			m_loop.halt();
			return core_application::exit(exit_code);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto delta_time() const noexcept -> duration { return m_delta_time; }

		ML_NODISCARD auto frame_index() const noexcept -> uint64 { return m_frame_index; }

		ML_NODISCARD auto fps_value() const noexcept -> float32 { return m_fps_value; }

		ML_NODISCARD auto fps_index() const noexcept -> size_t { return m_fps_index; }

		ML_NODISCARD auto fps_times() const noexcept -> ds::list<float32> const & { return m_fps_times; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_input() const noexcept { return const_cast<input_state *>(&m_input); }

		ML_NODISCARD auto get_loop() const noexcept { return const_cast<loop_system *>(&m_loop); }

		ML_NODISCARD auto get_window() const noexcept { return const_cast<native_window *>(&m_window); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_render_device() const noexcept -> gfx::render_device * { return m_render_device; }

		ML_NODISCARD auto get_render_context() const noexcept -> ref<gfx::render_context> const & { return m_render_device->get_context(); }

		void set_render_device(gfx::render_device * value) noexcept { m_render_device = value; }

		void set_render_context(ref<gfx::render_context> const & value) noexcept { m_render_device->set_context(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_dock_builder() const noexcept { return const_cast<ImGuiExt::Dockspace *>(&m_dock_builder); }

		ML_NODISCARD auto get_main_menu_bar() const noexcept { return const_cast<ImGuiExt::MainMenuBar *>(&m_main_menu_bar); }

		ML_NODISCARD auto get_imgui_context() const noexcept -> ImGuiContext * { return m_imgui_context; }

		void set_imgui_context(ImGuiContext * value) noexcept { m_imgui_context = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_active_scene() const noexcept -> ref<scene> const & { return m_active_scene; }

		void set_active_scene(ref<scene> const & value) noexcept { m_active_scene = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_enter();

		virtual void on_exit();

		virtual void on_idle();

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		loop_system				m_loop			; // main loop
		native_window			m_window		; // main window
		gfx::render_device *	m_render_device	; // render device
		ImGuiContext *			m_imgui_context	; // imgui context
		ImGuiExt::Dockspace		m_dock_builder	; // dock builder
		ImGuiExt::MainMenuBar	m_main_menu_bar	; // main menu bar
		ref<scene>				m_active_scene	; // active scene

		timer					m_loop_timer	; // loop timer
		duration				m_delta_time	; // delta time
		uint64					m_frame_index	; // frame index
		float32					m_fps_value		; // fps value
		float32					m_fps_accum		; // fps accumulator
		size_t					m_fps_index		; // fps index
		ds::list<float32>		m_fps_times		; // fps times
		input_state				m_input			; // input
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global gui_application
namespace ml::globals
{
	ML_decl_global(gui_application) get_global() noexcept;

	ML_decl_global(gui_application) set_global(gui_application * value) noexcept;
}

#endif // !_ML_GUI_APPLICATION_HPP_