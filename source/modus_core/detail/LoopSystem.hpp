#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/detail/List.hpp>
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

		virtual ~loop_system() noexcept = default;

		loop_system(allocator_type alloc = {}) noexcept
			: m_locked		{}
			, m_subsystems	{ alloc }
			, m_loopcond	{}
			, m_on_enter	{}
			, m_on_exit		{}
			, m_on_idle		{}
		{
		}

		explicit loop_system(loop_system const & other, allocator_type alloc = {})
			: m_locked		{}
			, m_subsystems	{ other.m_subsystems, alloc }
			, m_loopcond	{ other.m_loopcond }
			, m_on_enter	{ other.m_on_enter }
			, m_on_exit		{ other.m_on_exit }
			, m_on_idle		{ other.m_on_idle }
		{
		}

		explicit loop_system(loop_system && other, allocator_type alloc = {}) noexcept
			: loop_system{ alloc }
		{
			this->swap(std::move(other));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		loop_system & operator=(loop_system const & other) noexcept
		{
			loop_system temp{ other };
			this->swap(temp);
			return (*this);
		}

		loop_system & operator=(loop_system && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(loop_system & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_locked, other.m_locked);
				m_subsystems.swap(other.m_subsystems);
				m_loopcond.swap(m_loopcond);
				m_on_enter.swap(m_on_enter);
				m_on_exit.swap(m_on_exit);
				m_on_idle.swap(m_on_idle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process(bool recursive = true) noexcept
		{
			// lock
			if (!do_lock()) { return EXIT_FAILURE * 1; }
			
			// unlock
			ML_defer(&) { do_unlock(); };

			// enter
			do_invoke(&loop_system::m_on_enter, recursive);

			// exit
			ML_defer(&) { do_invoke(&loop_system::m_on_exit, recursive); };

			// idle
			if (!check_loop_condition()) { return EXIT_FAILURE * 2; }
			do { do_invoke(&loop_system::m_on_idle, recursive); }
			while (check_loop_condition());
			
			// good
			return EXIT_SUCCESS;
		}

		ML_NODISCARD int32_t operator()(bool recursive = true) noexcept
		{
			return this->process(recursive);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_loop_condition() const noexcept { return m_loopcond && m_loopcond(); }

		ML_NODISCARD auto get_loop_condition() const noexcept -> loop_condition const & { return m_loopcond; }

		ML_NODISCARD auto get_enter_callback() const noexcept -> loop_callback const & { return m_on_enter; }

		ML_NODISCARD auto get_exit_callback() const noexcept -> loop_callback const & { return m_on_exit; }

		ML_NODISCARD auto get_idle_callback() const noexcept -> loop_callback const & { return m_on_idle; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> auto set_loop_condition(Fn && fn, Args && ... args) noexcept -> loop_condition &
		{
			return this->do_bind(&loop_system::m_loopcond, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_enter_callback(Fn && fn, Args && ... args) -> loop_callback &
		{
			return this->do_bind(&loop_system::m_on_enter, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_exit_callback(Fn && fn, Args && ... args) -> loop_callback &
		{
			return this->do_bind(&loop_system::m_on_exit, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_idle_callback(Fn && fn, Args && ... args) -> loop_callback &
		{
			return this->do_bind(&loop_system::m_on_idle, ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

		auto delete_subsystem(subsystem const & value) noexcept -> iterator
		{
			if (auto const it{ find_subsystem(value) }
			; it == end()) { return it; }
			else { return m_subsystems.erase(it); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto subsystems() & noexcept -> subsystem_list & { return m_subsystems; }

		ML_NODISCARD auto subsystems() const & noexcept -> subsystem_list const & { return m_subsystems; }

		ML_NODISCARD auto subsystems() && noexcept -> subsystem_list && { return std::move(m_subsystems); }

		ML_NODISCARD auto operator[](size_t const i) & noexcept -> subsystem & { return m_subsystems[i]; }

		ML_NODISCARD auto operator[](size_t const i) const & noexcept -> subsystem const & { return m_subsystems[i]; }

		ML_NODISCARD auto operator[](size_t const i) && noexcept -> subsystem & { return std::move(m_subsystems[i]); }

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
		// lock internal
		bool do_lock() noexcept
		{
			return !m_locked && (m_locked = true);
		}

		// unlock internal
		bool do_unlock() noexcept
		{
			return m_locked && !(m_locked = false);
		}

		// bind internal
		template <class Member, class Fn, class ... Args
		> auto do_bind(Member loop_system::*mp, Fn && fn, Args && ... args) noexcept -> Member &
		{
			if constexpr (0 == sizeof...(args))
			{
				return (this->*mp) = ML_forward(fn);
			}
			else
			{
				return (this->*mp) = std::bind(ML_forward(fn), ML_forward(args)...);
			}
		}

		// invoke internal
		bool do_invoke(loop_callback loop_system::*mp, bool recursive) noexcept
		{
			bool const good{ mp && this->*mp };
			
			if (good) { std::invoke(this->*mp); }
			
			if (recursive) for (auto & e : *this) { e->do_invoke(mp, recursive); }

			return good;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool			m_locked	; // process locked
		subsystem_list	m_subsystems; // subsystem list
		loop_condition	m_loopcond	; // loop condition
		loop_callback	m_on_enter	, // enter callback
						m_on_exit	, // exit callback
						m_on_idle	; // idle callback

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LOOP_SYSTEM_HPP_