#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/engine/CoreApplication.hpp>
#include <modus_core/engine/InputManager.hpp>
#include <modus_core/graphics/Renderer.hpp>
#include <modus_core/graphics/Viewport.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/scene/Scene.hpp>
#include <modus_core/window/NativeWindow.hpp>

namespace ml
{
	// gui application
	struct ML_CORE_API gui_application : core_application, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using core_application::allocator_type;

		gui_application(int32 argc, char * argv[], json const & j = {}, allocator_type alloc = {});
		
		virtual ~gui_application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

		ML_NODISCARD auto total_time() const noexcept -> duration { return m_main_timer.elapsed(); }

		ML_NODISCARD auto delta_time() const noexcept -> duration { return m_loop_delta; }

		ML_NODISCARD auto frame_rate() const noexcept -> float32 { return m_fps_value; }

		ML_NODISCARD auto frame_count() const noexcept -> uint64 { return m_loop_index; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_input() const noexcept { return const_cast<input_manager *>(&m_input); }

		ML_NODISCARD auto get_loop() const noexcept { return const_cast<loop_system *>(&m_loop); }

		ML_NODISCARD auto get_window() const noexcept { return const_cast<native_window *>(&m_window); }

		ML_NODISCARD auto get_renderer() const noexcept { return const_cast<renderer *>(&m_renderer); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_imgui_context() const noexcept -> ImGuiContext * { return m_imgui.get(); }

		ML_NODISCARD auto get_dock_builder() const noexcept { return const_cast<ImGuiExt::Dockspace *>(&m_dock_builder); }

		ML_NODISCARD auto get_main_menu_bar() const noexcept { return const_cast<ImGuiExt::MainMenuBar *>(&m_main_menu_bar); }

		void set_imgui_context(ImGuiContext * value) noexcept { m_imgui.reset(value); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_active_scene() const noexcept -> ds::ref<scene> const & { return m_scene; }

		void set_active_scene(ds::ref<scene> const & value) noexcept { if (m_scene != value) { m_scene = value; } }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_enter();

		virtual void on_exit();

		virtual void on_idle();

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_bus				m_dispatcher	; // event bus
		native_window			m_window		; // window
		renderer				m_renderer		; // renderer

		ds::scary<ImGuiContext>	m_imgui			; // imgui context
		ImGuiExt::Dockspace		m_dock_builder	; // dock builder
		ImGuiExt::MainMenuBar	m_main_menu_bar	; // main menu bar

		loop_system				m_loop			; // main loop
		ds::ref<scene>			m_scene			; // active scene

		timer					m_main_timer	, // main timer
								m_loop_timer	; // idle timer
		duration				m_loop_delta	; // idle delta
		uint64					m_loop_index	; // idle index
		float32					m_fps_value		; // fps value
		float32					m_fps_accum		; // fps accumulator
		size_t					m_fps_index		; // fps index
		ds::list<float32>		m_fps_times		; // fps times
		input_manager			m_input			; // input manager
		
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