#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/Runtime.hpp>

namespace ml
{
	// main loop
	struct ML_CORE_API main_loop : runtime_listener<main_loop>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using loop_condition = typename ds::method<bool()>;

		using subsystem = typename ds::shared_ptr<main_loop>;

		using subsystem_list = typename ds::list<subsystem>;

		using iterator = typename subsystem_list::iterator;
		
		using const_iterator = typename subsystem_list::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class Arg0, class ... Args
		> main_loop(runtime_api * api, Fn && fn, Arg0 && arg0, Args && ... args) noexcept
			: main_loop{ api, std::bind(ML_forward(fn), ML_forward(arg0), ML_forward(args)...) }
		{
		}

		main_loop(runtime_api * api, loop_condition const & loopcond = {}) noexcept;

		virtual ~main_loop() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool locked() const noexcept {
			return m_locked;
		}

		ML_NODISCARD bool check_condition() const noexcept {
			return m_loopcond && std::invoke(m_loopcond);
		}

		ML_NODISCARD auto get_condition() const noexcept -> loop_condition const & {
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto set_condition(Fn && fn, Args && ... args) noexcept -> loop_condition & {
			return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto delete_subsystem(subsystem const & value) -> iterator
		{
			if (auto const it{ std::find(begin(), end(), value) }
			; it == end()) { return it; }
			else
			{
				return m_subsystems.erase(it);
			}
		}

		template <class Derived, class ... Args
		> auto new_subsystem(Args && ... args) noexcept
		{
			static_assert(!std::is_same_v<main_loop, Derived>);

			static_assert(std::is_base_of_v<main_loop, Derived>);

			return std::static_pointer_cast<Derived>(m_subsystems.emplace_back
			(
				get_memory()->make_ref<Derived>(get_api(), ML_forward(args)...)
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_subsystems() noexcept -> subsystem_list & {
			return m_subsystems;
		}

		ML_NODISCARD auto get_subsystems() const noexcept -> subsystem_list const & {
			return m_subsystems;
		}

		ML_NODISCARD auto begin() noexcept -> iterator { return m_subsystems.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_subsystems.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_subsystems.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_subsystems.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_subsystems.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_subsystems.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		bool lock() noexcept { return !m_locked && (m_locked = true); }

		bool unlock() noexcept { return m_locked && !(m_locked = false); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_enter() = 0;
		
		virtual void on_exit() = 0;

		virtual void on_idle() = 0;

		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static void begin_idle(runtime_io * io) noexcept {
			io->loop_timer.restart();
			auto const dt{ (float_t)io->delta_time.count() };
			io->fps_accum += dt - io->fps_times[io->fps_index];
			io->fps_times[io->fps_index] = dt;
			io->fps_index = (io->fps_index + 1) % io->fps_times.size();
			io->fps = (0.f < io->fps_accum)
				? 1.f / (io->fps_accum / (float_t)io->fps_times.size())
				: FLT_MAX;
		}

		static void end_idle(runtime_io * io) noexcept {
			++io->frame_count;
			io->delta_time = io->loop_timer.elapsed();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool			m_locked	; // running
		loop_condition	m_loopcond	; // loop condition
		subsystem_list	m_subsystems; // subsystem list

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global player loop
namespace ml::globals
{
	template <> ML_NODISCARD ML_CORE_API main_loop * get() noexcept;

	template <> ML_CORE_API main_loop * set(main_loop * value) noexcept;
}

#endif // !_ML_MAIN_LOOP_HPP_