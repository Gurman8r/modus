#ifndef _ML_DEFAULT_APP_
#define _ML_DEFAULT_APP_

#include <modus_core/runtime/Application.hpp>
#include <modus_core/imgui/ImGuiExt.hpp>

namespace ml
{
	// default app
	struct ML_CORE_API default_app final : application
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit default_app(runtime_api * api) noexcept;

		~default_app() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		void on_enter();

		void on_exit();

		void on_idle();

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::manual<ImGuiContext>	m_imgui		; // imgui context
		ImGuiExt::Dockspace			m_dockspace	; // imgui dockspace

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto default_benchmarks(runtime_io & io) noexcept
		{
			io.loop_timer.restart();
			auto const dt{ (float_t)io.delta_time.count() };
			io.fps_accum += dt - io.fps_times[io.fps_index];
			io.fps_times[io.fps_index] = dt;
			io.fps_index = (io.fps_index + 1) % io.fps_times.size();
			io.fps = (0.f < io.fps_accum)
				? 1.f / (io.fps_accum / (float_t)io.fps_times.size())
				: FLT_MAX;
			return ML_defer_ex(&io) {
				++io.frame_count;
				io.delta_time = io.loop_timer.elapsed();
			};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_DEFAULT_APP_