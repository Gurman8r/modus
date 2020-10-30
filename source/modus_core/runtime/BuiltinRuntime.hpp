#ifndef _ML_BUILTIN_RUNTIME_HPP_
#define _ML_BUILTIN_RUNTIME_HPP_

#include <modus_core/runtime/LoopSystem.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// builtin runtime
	struct ML_CORE_API builtin_runtime final : loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit builtin_runtime(runtime_api * api) noexcept;

		~builtin_runtime() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void on_process_enter() override;

		void on_process_exit() override;

		void on_process_idle() override;

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace			m_dockspace	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto do_benchmarks(runtime_io * io) noexcept {
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

#endif // !_ML_BUILTIN_RUNTIME_HPP_