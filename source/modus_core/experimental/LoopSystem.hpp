#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/detail/Duration.hpp>
#include <modus_core/detail/Method.hpp>
#include <modus_core/system/Memory.hpp>

namespace ml
{
	// loop system
	struct ML_CORE_API loop_system : trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte>;
		using loop_condition			= typename method<bool()>;
		using enter_callback			= typename method<void()>;
		using exit_callback				= typename method<void()>;
		using idle_callback				= typename method<void()>;
		using subsystem					= typename ref<loop_system>;
		using subsystem_list			= typename list<subsystem>;
		using iterator					= typename subsystem_list::iterator;
		using const_iterator			= typename subsystem_list::const_iterator;
		using reverse_iterator			= typename subsystem_list::reverse_iterator;
		using const_reverse_iterator	= typename subsystem_list::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~loop_system() noexcept override = default;

		loop_system(allocator_type alloc = {}) noexcept
			: m_running		{}
			, m_subsystems	{ alloc }
			, m_condition	{}
			, m_on_enter	{}
			, m_on_exit		{}
			, m_on_idle		{}
		{
		}

		explicit loop_system(loop_system const & other, allocator_type alloc = {})
			: m_running		{}
			, m_subsystems	{ other.m_subsystems, alloc }
			, m_condition	{ other.m_condition }
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
				std::swap(m_running, other.m_running);
				m_subsystems.swap(other.m_subsystems);
				m_condition.swap(m_condition);
				m_on_enter.swap(m_on_enter);
				m_on_exit.swap(m_on_exit);
				m_on_idle.swap(m_on_idle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool running() const noexcept
		{
			return m_running;
		}

		template <bool Recursive = true
		> int32 loop() noexcept
		{
			// lock
			if (m_running) { return EXIT_FAILURE * 1; }
			else { m_running = true; } ML_defer(&) { m_running = false; };

			// enter
			this->run_enter_callback<Recursive>();
			
			// exit
			ML_defer(&) { this->run_exit_callback<Recursive>(); };
			
			// idle
			if (!this->check_condition()) { return EXIT_FAILURE * 2; }
			do { this->run_idle_callback<Recursive>(); }
			while (this->check_condition());
			return EXIT_SUCCESS;
		}

		template <bool Recursive = true
		> void halt() noexcept
		{
			m_running = false;

			if constexpr (Recursive) for (subsystem & e : m_subsystems)
			{
				e->halt();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool check_condition() const noexcept
		{
			return m_running && m_condition && m_condition();
		}

		template <bool Recursive = false, bool Reverse = false
		> void run_enter_callback() noexcept
		{
			loop_system::run<Recursive, Reverse>(&loop_system::m_on_enter, this);
		}

		template <bool Recursive = false, bool Reverse = true
		> void run_exit_callback() noexcept
		{
			loop_system::run<Recursive, Reverse>(&loop_system::m_on_exit, this);
		}

		template <bool Recursive = false, bool Reverse = false
		> void run_idle_callback() noexcept
		{
			loop_system::run<Recursive, Reverse>(&loop_system::m_on_idle, this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_condition() const noexcept -> loop_condition const & { return m_condition; }

		ML_NODISCARD auto get_enter_callback() const noexcept -> enter_callback const & { return m_on_enter; }

		ML_NODISCARD auto get_exit_callback() const noexcept -> exit_callback const & { return m_on_exit; }

		ML_NODISCARD auto get_idle_callback() const noexcept -> idle_callback const & { return m_on_idle; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> auto set_condition(Fn && fn, Args && ... args) noexcept -> loop_condition
		{
			return util::chain(m_condition, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_enter_callback(Fn && fn, Args && ... args) -> enter_callback
		{
			return util::chain(m_on_enter, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_exit_callback(Fn && fn, Args && ... args) -> exit_callback
		{
			return util::chain(m_on_exit, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_idle_callback(Fn && fn, Args && ... args) -> idle_callback
		{
			return util::chain(m_on_idle, ML_forward(fn), ML_forward(args)...);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto subsystems() & noexcept -> subsystem_list & { return m_subsystems; }

		ML_NODISCARD auto subsystems() const & noexcept -> subsystem_list const & { return m_subsystems; }

		ML_NODISCARD auto subsystems() && noexcept -> subsystem_list && { return std::move(m_subsystems); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto add_subsystem(subsystem const & value) noexcept -> subsystem
		{
			if (this == value.get()) { return nullptr; }
			else if (auto const it{ find_subsystem(value) }
			; it != m_subsystems.end()) { return *it; }
			else { return m_subsystems.emplace_back(value); }
		}

		auto delete_subsystem(subsystem const & value) noexcept -> iterator
		{
			if (this == value.get()) { return m_subsystems.end(); }
			else if (auto const it{ find_subsystem(value) }
			; it == m_subsystems.end()) { return it; }
			else { return m_subsystems.erase(it); }
		}

		template <class Derived = loop_system, class ... Args
		> auto new_subsystem(Args && ... args) noexcept -> ref<Derived>
		{
			static_assert(std::is_base_of_v<loop_system, Derived>);

			return std::static_pointer_cast<Derived>(m_subsystems.emplace_back
			(
				_ML make_ref<Derived>(ML_forward(args)...)
			));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool has_subsystem(loop_system const * value) const noexcept
		{
			return m_subsystems.end() != find_subsystem_if([value](subsystem const & e) noexcept
			{
				return value == e.get();
			});
		}

		ML_NODISCARD bool has_subsystem(subsystem const & value) const noexcept
		{
			return m_subsystems.end() != find_subsystem(value);
		}

		ML_NODISCARD auto find_subsystem(subsystem const & value) noexcept -> iterator
		{
			return std::find(m_subsystems.begin(), m_subsystems.end(), value);
		}

		ML_NODISCARD auto find_subsystem(subsystem const & value) const noexcept -> const_iterator
		{
			return std::find(m_subsystems.cbegin(), m_subsystems.cend(), value);
		}

		template <class Pr
		> ML_NODISCARD auto find_subsystem_if(Pr && pr) noexcept -> iterator
		{
			return std::find_if(m_subsystems.begin(), m_subsystems.end(), ML_forward(pr));
		}

		template <class Pr
		> ML_NODISCARD auto find_subsystem_if(Pr && pr) const noexcept -> const_iterator
		{
			return std::find_if(m_subsystems.cbegin(), m_subsystems.cend(), ML_forward(pr));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		// execute member function pointer
		template <
			bool Recursive = false,
			bool Reverse = false,
			class D, class C, class ... Args
		> static void run(D C::*mp, C * self, Args && ... args) noexcept
		{
			static_assert(std::is_base_of_v<loop_system, C>);

			if (!self || !mp) { return; } // nothing to do

			auto run_self{ std::bind(self->*mp, ML_forward(args)...) };

			if constexpr (!Reverse) { run_self(); }

			if constexpr (Recursive)
			{
				using Iter = std::conditional_t<!Reverse,
					subsystem_list::const_iterator,
					subsystem_list::const_reverse_iterator>;

				auto first = [&]() noexcept -> Iter {
					if constexpr (Reverse) { return self->m_subsystems.rbegin(); }
					else { return self->m_subsystems.begin(); }
				};

				auto last = [&]() noexcept -> Iter {
					if constexpr (Reverse) { return self->m_subsystems.rend(); }
					else { return self->m_subsystems.end(); }
				};

				std::for_each(first(), last(), [&](subsystem const & e) noexcept
				{
					if constexpr (std::is_same_v<C, loop_system>)
					{
						loop_system::run<true, Reverse>(mp, self, ML_forward(args)...); // skip cast
					}
					else
					{
						loop_system::run<true, Reverse>(mp, dynamic_cast<C *>(e.get()), ML_forward(args)...);
					}
				});
			}

			if constexpr (Reverse) { run_self(); }
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool					m_running		; // running
		subsystem_list			m_subsystems	; // subsystem list
		loop_condition			m_condition		; // loop condition
		enter_callback			m_on_enter		; // enter callback
		exit_callback			m_on_exit		; // exit callback
		idle_callback			m_on_idle		; // idle callback

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LOOP_SYSTEM_HPP_