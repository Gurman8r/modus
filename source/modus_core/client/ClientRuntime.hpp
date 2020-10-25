#ifndef _ML_CLIENT_RUNTIME_HPP_
#define _ML_CLIENT_RUNTIME_HPP_

#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/client/PluginManager.hpp>

namespace ml
{
	struct client_runtime;

	ML_NODISCARD ML_CORE_API client_runtime * get_default_runtime() noexcept;

	ML_CORE_API client_runtime * set_default_runtime(client_runtime * value) noexcept;
}

namespace ml
{
	// loop condition
	ML_alias loop_condition_fn = typename ds::method<int32_t(void)>;

	// client runtime
	struct ML_CORE_API client_runtime final : client_listener<client_runtime>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit client_runtime(client_context * context);

		~client_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// LOOP

		ML_NODISCARD int32_t idle();

		ML_NODISCARD bool check_loop_condition() const noexcept { return m_loopcond && m_loopcond(); }

		ML_NODISCARD loop_condition_fn const & get_loop_condition() const noexcept { return m_loopcond; }

		void set_loop_condition(loop_condition_fn const & value) noexcept { m_loopcond = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// IMGUI

		ML_NODISCARD auto get_imgui() const noexcept -> manual<ImGuiContext> const & { return m_imgui; }

		ML_NODISCARD auto get_dockspace() noexcept -> ImGuiExt::Dockspace & { return m_dock; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		// PLUGINS

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void do_startup(client_context * ctx);

		void do_shutdown();

		void do_gui();

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool					m_idling	; // run lock
		loop_condition_fn		m_loopcond	; // loop condition
		manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace		m_dock		; // imgui dockspace
		plugin_manager			m_plugins	; // plugin manager

		ML_NODISCARD static auto do_update_timers(client_io & io) noexcept
		{
			io.loop_timer.restart();
			auto const dt{ (float_t)io.delta_time.count() };
			io.fps_accum += dt - io.fps_times[io.fps_index];
			io.fps_times[io.fps_index] = dt;
			io.fps_index = (io.fps_index + 1) % io.fps_times.size();
			io.fps = (0.f < io.fps_accum)
				? 1.f / (io.fps_accum / (float_t)io.fps_times.size())
				: FLT_MAX;
			return ML_defer_ex(&) {
				++io.frame_count;
				io.delta_time = io.loop_timer.elapsed();
			};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CLIENT_RUNTIME_HPP_