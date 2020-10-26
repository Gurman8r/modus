#ifndef _ML_RUNTIME_HPP_
#define _ML_RUNTIME_HPP_

#include <modus_core/graphics/RenderWindow.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>
#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	struct runtime_context;

	ML_NODISCARD ML_CORE_API runtime_context * get_global_runtime() noexcept;

	ML_CORE_API runtime_context * set_global_runtime(runtime_context * value) noexcept;

	// runtime context
	struct ML_CORE_API runtime_context final : runtime_listener<runtime_context>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		explicit runtime_context(runtime_api * api);

		~runtime_context() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t idle();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		ML_NODISCARD auto get_imgui() const noexcept -> ImGuiContext * { return m_imgui.get(); }

		ML_NODISCARD auto get_docker() noexcept -> ImGuiExt::Dockspace & { return m_dockspace; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_condition() const noexcept {
			return std::invoke(m_loopcond);
		}

		ML_NODISCARD auto const & get_loop_condition() const noexcept {
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto & set_loop_condition(Fn && fn, Args && ... args) noexcept {
			return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void on_event(event && value) override;

		void do_startup();

		void do_shutdown();

		void do_idle();

		void do_imgui();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool					m_running	; // running
		ds::method<bool()>		m_loopcond	; // loop condition
		plugin_manager			m_plugins	; // plugin manager
		manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace		m_dockspace	; // imgui dockspace

		ML_NODISCARD static auto process_timers(runtime_io * io) noexcept
		{
			io->loop_timer.restart();
			auto const dt{ (float_t)io->delta_time.count() };
			io->fps_accum += dt - io->fps_times[io->fps_index];
			io->fps_times[io->fps_index] = dt;
			io->fps_index = (io->fps_index + 1) % io->fps_times.size();
			io->fps = (0.f < io->fps_accum)
				? 1.f / (io->fps_accum / (float_t)io->fps_times.size())
				: FLT_MAX;
			return ML_defer_ex(io) {
				++io->frame_count;
				io->delta_time = io->loop_timer.elapsed();
			};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RUNTIME_LOOP_HPP_