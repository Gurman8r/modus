#ifndef _ML_RUNTIME_CONTEXT_HPP_
#define _ML_RUNTIME_CONTEXT_HPP_

#include <modus_core/runtime/PluginManager.hpp>
#include <modus_core/graphics/RenderWindow.hpp>

namespace ml
{
	struct runtime_context;

	ML_NODISCARD ML_CORE_API runtime_context * get_global_runtime() noexcept;

	ML_CORE_API runtime_context * set_global_runtime(runtime_context * value) noexcept;

	struct ML_CORE_API runtime_context : runtime_listener<runtime_context>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using loop_condition_fn = typename ds::method<bool()>;

		explicit runtime_context(runtime_api * api) noexcept;

		virtual ~runtime_context() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t idle();

		ML_NODISCARD bool check_loop_condition() const noexcept {
			return m_loopcond && m_loopcond();
		}

		ML_NODISCARD auto const & get_loop_condition() const noexcept {
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto & set_loop_condition(Fn && fn, Args && ... args) noexcept {
			return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_enter() = 0;
		
		virtual void on_exit() = 0;

		virtual void on_idle() = 0;

		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool				m_running	; // running
		loop_condition_fn	m_loopcond	; // loop condition
		plugin_manager		m_plugins	; // plugin manager

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
			return ML_defer_ex(io)
			{
				++io->frame_count;
				io->delta_time = io->loop_timer.elapsed();
			};
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_RUNTIME_CONTEXT_HPP_