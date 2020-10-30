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

		loop_system(runtime_api * api, loop_condition const & loopcond = {}) noexcept;

		template <class Fn, class Arg0, class ... Args
		> loop_system(runtime_api * api, Fn && fn, Arg0 && arg0, Args && ... args) noexcept
			: loop_system{ api, std::bind(ML_forward(fn), ML_forward(arg0), ML_forward(args)...) }
		{
		}

		virtual ~loop_system() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process() noexcept
		{
			if (!lock()) { return EXIT_FAILURE * 1; } ML_defer(&) { unlock(); };

			on_process_enter(); ML_defer(&) { on_process_exit(); };

			if (!check_condition()) { return EXIT_FAILURE * 2; }

			do { on_process_idle(); } while (check_condition());

			return EXIT_SUCCESS;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_condition() const noexcept {
			return m_loopcond && m_loopcond();
		}

		ML_NODISCARD auto get_condition() const noexcept -> loop_condition const & {
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto set_condition(Fn && fn, Args && ... args) noexcept -> loop_condition & {
			return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_subsystems() noexcept -> subsystem_list & {
			return m_subsystems;
		}

		ML_NODISCARD auto get_subsystems() const noexcept -> subsystem_list const & {
			return m_subsystems;
		}

		template <class Derived, class ... Args
		> auto new_subsystem(Args && ... args) noexcept -> ds::shared<Derived>
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
			if (auto const it{ std::find(begin(), end(), value) }; it == end()) { return it; }
			else
			{
				return m_subsystems.erase(it);
			}
		}

		ML_NODISCARD auto begin() noexcept -> iterator { return m_subsystems.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_subsystems.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_subsystems.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_subsystems.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_subsystems.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_subsystems.cend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		bool lock() noexcept {
			return !m_locked && (m_locked = true);
		}

		bool unlock() noexcept {
			return m_locked && !(m_locked = false);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_process_enter()
		{
			for (auto & e : *this) { e->on_process_enter(); }
		}

		virtual void on_process_exit()
		{
			for (auto & e : *this) { e->on_process_exit(); }
		}

		virtual void on_process_idle()
		{
			for (auto & e : *this) { e->on_process_idle(); }
		}

		virtual void on_event(event && ev) override
		{
			for (auto & e : *this) { e->on_event(ML_forward(ev)); }
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