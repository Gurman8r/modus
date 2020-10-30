#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/detail/List.hpp>
#include <modus_core/detail/Pointer.hpp>
#include <modus_core/detail/Method.hpp>

namespace ml
{
	// loop system
	struct ML_CORE_API loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte_t>;
		using loop_condition			= typename ds::method<bool()>;
		using loop_callback				= typename ds::method<void()>;
		using subsystem					= typename ds::ref<loop_system>;
		using subsystem_list			= typename ds::list<subsystem>;
		using iterator					= typename subsystem_list::iterator;
		using const_iterator			= typename subsystem_list::const_iterator;
		using reverse_iterator			= typename subsystem_list::reverse_iterator;
		using const_reverse_iterator	= typename subsystem_list::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		loop_system(allocator_type alloc = {}) noexcept
			: m_locked		{}
			, m_subsystems	{ alloc }
			, m_loopcond	{}
			, m_on_enter	{}
			, m_on_exit		{}
			, m_on_idle		{}
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t operator()() noexcept { return process(); }

		ML_NODISCARD int32_t process(bool recursive = true) noexcept
		{
			if (!lock()) { return EXIT_FAILURE * 1; } ML_defer(&) { unlock(); };

			process_enter(recursive); ML_defer(&) { process_exit(recursive); };

			if (!check_condition()) { return EXIT_FAILURE * 2; }

			do { process_idle(recursive); } while (check_condition());

			return EXIT_SUCCESS;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_condition() const noexcept { return m_loopcond && m_loopcond(); }

		ML_NODISCARD auto get_condition() const noexcept -> loop_condition const & { return m_loopcond; }

		ML_NODISCARD auto get_enter() const noexcept -> loop_callback const & { return m_on_enter; }

		ML_NODISCARD auto get_exit() const noexcept -> loop_callback const & { return m_on_exit; }

		ML_NODISCARD auto get_idle() const noexcept -> loop_callback const & { return m_on_idle; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> auto bind_condition(Fn && fn, Args && ... args) noexcept -> loop_condition &
		{
			if constexpr (0 == sizeof...(args)) { return m_loopcond = ML_forward(fn); }
			else { return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...); }
		}

		template <class Fn, class ... Args
		> auto bind_enter(Fn && fn, Args && ... args) -> loop_callback &
		{
			if constexpr (0 == sizeof...(args)) { return m_on_enter = ML_forward(fn); }
			else { return m_on_enter = std::bind(ML_forward(fn), ML_forward(args)...); }
		}

		template <class Fn, class ... Args
		> auto bind_exit(Fn && fn, Args && ... args) -> loop_callback &
		{
			if constexpr (0 == sizeof...(args)) { return m_on_exit = ML_forward(fn); }
			else { return m_on_exit = std::bind(ML_forward(fn), ML_forward(args)...); }
		}

		template <class Fn, class ... Args
		> auto bind_idle(Fn && fn, Args && ... args) -> loop_callback &
		{
			if constexpr (0 == sizeof...(args)) { return m_on_idle = ML_forward(fn); }
			else { return m_on_idle = std::bind(ML_forward(fn), ML_forward(args)...); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto subsystems() & noexcept -> subsystem_list & { return m_subsystems; }

		ML_NODISCARD auto subsystems() const & noexcept -> subsystem_list const & { return m_subsystems; }

		ML_NODISCARD auto subsystems() && noexcept -> subsystem_list && { return std::move(m_subsystems); }

		ML_NODISCARD auto operator[](size_t const i) & noexcept -> subsystem & { return m_subsystems[i]; }

		ML_NODISCARD auto operator[](size_t const i) const && noexcept -> subsystem const & { return m_subsystems[i]; }

		ML_NODISCARD auto operator[](size_t const i) && noexcept -> subsystem & { return std::move(m_subsystems[i]); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto delete_subsystem(iterator it) -> iterator
		{
			return (it == end()) ? it : m_subsystems.erase(it);
		}

		auto delete_subsystem(subsystem const & value) noexcept -> iterator
		{
			return delete_subsystem(find_subsystem(value));
		}

		ML_NODISCARD auto find_subsystem(subsystem const & value) noexcept -> iterator
		{
			return (this == value.get()) ? end() : std::find(begin(), end(), value);
		}

		ML_NODISCARD auto find_subsystem(subsystem const & value) const noexcept -> const_iterator
		{
			return (this == value.get()) ? end() : std::find(begin(), end(), value);
		}

		bool has_subsystem(subsystem const & value) const noexcept
		{
			return find_subsystem(value) != end();
		}

		template <class Derived = loop_system, class ... Args
		> auto new_subsystem(Args && ... args) noexcept -> ds::ref<Derived>
		{
			static_assert(std::is_base_of_v<loop_system, Derived>, "?");

			return std::static_pointer_cast<Derived>(m_subsystems.emplace_back
			(
				std::make_shared<Derived>(ML_forward(args)...)
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto begin() noexcept -> iterator { return m_subsystems.begin(); }

		ML_NODISCARD auto begin() const noexcept -> const_iterator { return m_subsystems.begin(); }
		
		ML_NODISCARD auto cbegin() const noexcept -> const_iterator { return m_subsystems.cbegin(); }

		ML_NODISCARD auto end() noexcept -> iterator { return m_subsystems.end(); }

		ML_NODISCARD auto end() const noexcept -> const_iterator { return m_subsystems.end(); }

		ML_NODISCARD auto cend() const noexcept -> const_iterator { return m_subsystems.cend(); }

		ML_NODISCARD auto rbegin() noexcept -> reverse_iterator { return m_subsystems.rbegin(); }

		ML_NODISCARD auto rbegin() const noexcept -> const_reverse_iterator { return m_subsystems.rbegin(); }

		ML_NODISCARD auto crbegin() const noexcept -> const_reverse_iterator { return m_subsystems.crbegin(); }

		ML_NODISCARD auto rend() noexcept -> reverse_iterator { return m_subsystems.rend(); }

		ML_NODISCARD auto rend() const noexcept -> const_reverse_iterator { return m_subsystems.rend(); }

		ML_NODISCARD auto crend() const noexcept -> const_reverse_iterator { return m_subsystems.crend(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		bool lock() noexcept
		{
			return !m_locked && (m_locked = true);
		}

		bool unlock() noexcept
		{
			return m_locked && !(m_locked = false);
		}

		void process_enter(bool recursive) noexcept
		{
			if (m_on_enter) { m_on_enter(); }
			if (recursive) {
				for (auto & e : *this) {
					e->process_enter(recursive);
				}
			}
		}

		void process_exit(bool recursive) noexcept
		{
			if (m_on_exit) { m_on_exit(); }
			if (recursive) {
				for (auto & e : *this) {
					e->process_exit(recursive);
				}
			}
		}

		void process_idle(bool recursive) noexcept
		{
			if (m_on_idle) { m_on_idle(); }
			if (recursive) {
				for (auto & e : *this) {
					e->process_idle(recursive);
				}
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool			m_locked	; // process lock
		subsystem_list	m_subsystems; // subsystem list
		loop_condition	m_loopcond	; // loop condition
		loop_callback	m_on_enter	, // enter callback
						m_on_exit	, // exit callback
						m_on_idle	; // idle callback

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LOOP_SYSTEM_HPP_