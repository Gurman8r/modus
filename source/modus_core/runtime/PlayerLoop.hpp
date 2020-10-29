#ifndef _ML_PLAYER_LOOP_HPP_
#define _ML_PLAYER_LOOP_HPP_

#include <modus_core/runtime/Runtime.hpp>

namespace ml
{
	// player loop
	struct ML_CORE_API player_loop : runtime_listener<player_loop>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using loop_condition = typename ds::method<bool()>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit player_loop(runtime_api * api) noexcept;

		virtual ~player_loop() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process() noexcept;

		ML_NODISCARD bool is_running() const noexcept { return m_running; }
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_loop_condition() const noexcept {
			return m_loopcond && m_loopcond();
		}

		ML_NODISCARD auto get_loop_condition() const noexcept -> loop_condition const & {
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto set_loop_condition(Fn && fn, Args && ... args) noexcept -> loop_condition & {
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
		bool			m_running	; // is running
		loop_condition	m_loopcond	; // loop condition

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

// global player loop
namespace ml::globals
{
	template <> ML_NODISCARD ML_CORE_API player_loop * get() noexcept;

	template <> ML_CORE_API player_loop * set(player_loop * value) noexcept;
}

#endif // !_ML_PLAYER_LOOP_HPP_