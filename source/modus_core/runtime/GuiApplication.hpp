#ifndef _ML_GUI_APPLICATION_HPP_
#define _ML_GUI_APPLICATION_HPP_

#include <modus_core/runtime/CoreApplication.hpp>
#include <modus_core/graphics/RenderTarget.hpp>
#include <modus_core/gui/Dockspace.hpp>
#include <modus_core/gui/PanelWindow.hpp>
#include <modus_core/window/NativeWindow.hpp>

namespace ml
{
	// fps tracker
	struct ML_NODISCARD fps_tracker final : non_copyable, trackable
	{
		float32			value; // value
		float32			accum; // accumulator
		size_t			index; // index
		list<float32>	times; // times

		fps_tracker(size_t count = 0, pmr::polymorphic_allocator<byte> alloc = {}) noexcept
			: value{}, accum{}, index{}, times{ count, 0.f, alloc }
		{
		}

		void update(duration dt) noexcept
		{
			accum += dt - times[index];
			times[index] = dt;
			index = (index + 1) % times.size();
			value = (0.f < accum) ? (1.f / (accum / (float32)times.size())) : FLT_MAX;
		}
	};
}

namespace ml
{
	// input state
	struct ML_NODISCARD input_state final
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		char last_char;
		
		bool is_shift, is_ctrl, is_alt, is_super;

		float32 mouse_wheel;

		vec2 mouse_pos, last_mouse_pos, mouse_delta;

		bool mouse_down[mouse_button_MAX], keys_down[keycode_MAX];
		
		float32 mouse_down_duration[mouse_button_MAX], keys_down_duration[keycode_MAX];

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		input_state() noexcept = default;
		input_state(input_state const &) = default;
		input_state(input_state &&) noexcept = default;
		input_state & operator=(input_state const &) = default;
		input_state & operator=(input_state &&) noexcept = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

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
		virtual int32 run() override;

		virtual void exit(int32 exit_code = EXIT_SUCCESS) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_delta_time() const noexcept -> duration { return m_delta_time; }

		ML_NODISCARD auto get_fps() const noexcept { return const_cast<fps_tracker *>(&m_fps); }

		ML_NODISCARD auto get_frame() const noexcept -> uint64 { return m_frame_index; }

		ML_NODISCARD auto get_input() const noexcept { return const_cast<input_state *>(&m_input); }

		ML_NODISCARD auto get_main_window() const noexcept { return const_cast<native_window *>(&m_window); }

		ML_NODISCARD auto get_render_device() const noexcept -> scary<gfx::render_device> const & { return m_render_device; }

		ML_NODISCARD auto get_render_context() const noexcept -> ref<gfx::render_context> const & { return m_render_device->get_context(); }

		ML_NODISCARD auto get_imgui() const noexcept -> scary<ImGuiContext> const & { return m_imgui; }

		ML_NODISCARD auto get_dockspace() const noexcept { return const_cast<ImGuiExt::Dockspace *>(&m_dockspace); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_startup();

		virtual void on_shutdown();

		virtual void on_idle(duration dt);

		virtual void on_gui();

		virtual void on_end_frame();

		virtual void on_event(event const & value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		native_window				m_window		; // main window
		scary<gfx::render_device>	m_render_device	; // render device
		scary<ImGuiContext>			m_imgui			; // imgui context
		ImGuiExt::Dockspace			m_dockspace		; // dockspace
		
		timer			m_loop_timer	; // loop timer
		duration		m_delta_time	; // delta time
		uint64			m_frame_index	; // frame index
		fps_tracker		m_fps			; // fps tracker
		input_state		m_input			; // input state
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global gui_application
namespace ml::globals
{
	ML_decl_global(gui_application) get_global();

	ML_decl_global(gui_application) set_global(gui_application *);
}

#endif // !_ML_GUI_APPLICATION_HPP_