#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/runtime/Runtime.hpp>

namespace ml
{
	// loop system
	struct ML_CORE_API loop_system : runtime_listener<loop_system>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using loop_condition	= typename ds::method<bool()>;

		using subsystem			= typename ds::shared<loop_system>;

		using subsystem_list	= typename ds::list<subsystem>;

		using iterator			= typename subsystem_list::iterator;
		
		using const_iterator	= typename subsystem_list::const_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class Arg0, class ... Args
		> loop_system(runtime_api * api, Fn && fn, Arg0 && arg0, Args && ... args) noexcept
			: loop_system{ api, std::bind(ML_forward(fn), ML_forward(arg0), ML_forward(args)...) }
		{
		}

		loop_system(runtime_api * api, loop_condition const & loopcond = {}) noexcept;

		virtual ~loop_system() noexcept override;

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

		template <class Derived, class ... Args
		> auto new_subsystem(Args && ... args) noexcept
		{
			static_assert(!std::is_same_v<loop_system, Derived>);

			static_assert(std::is_base_of_v<loop_system, Derived>);

			return std::static_pointer_cast<Derived>(m_subsystems.emplace_back
			(
				get_memory()->make_ref<Derived>(get_api(), ML_forward(args)...)
			));
		}

		auto delete_subsystem(subsystem const & value) -> iterator
		{
			if (auto const it{ std::find(begin(), end(), value) }
			; it == end()) { return it; }
			else { return m_subsystems.erase(it); }
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

		virtual void on_process_enter() = 0;
		
		virtual void on_process_exit() = 0;

		virtual void on_process_idle() = 0;

		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static auto do_global_benchmarks(runtime_io * io) noexcept {
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
	template <> ML_NODISCARD ML_CORE_API loop_system * get() noexcept;

	template <> ML_CORE_API loop_system * set(loop_system * value) noexcept;
}

#endif // !_ML_LOOP_SYSTEM_HPP_