#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/PluginManager.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	// main loop
	struct ML_CORE_API main_loop : runtime_listener<main_loop>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit main_loop(runtime_api * api) noexcept;

		virtual ~main_loop() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_condition() const noexcept
		{
			return m_loopcond && m_loopcond();
		}

		ML_NODISCARD auto get_condition() const noexcept -> auto const &
		{
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto set_condition(Fn && fn, Args && ... args) noexcept -> auto &
		{
			return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_enter() = 0;
		
		virtual void on_exit() = 0;

		virtual void on_idle() = 0;

		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool				m_running	; // running
		ds::method<bool()>	m_loopcond	; // loop condition

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto do_benchmarks(runtime_io * io) noexcept
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

// global main loop
namespace ml::globals
{
	template <> ML_NODISCARD ML_CORE_API main_loop * get() noexcept;

	template <> ML_CORE_API main_loop * set(main_loop * value) noexcept;
}

#endif // !_ML_MAIN_LOOP_HPP_