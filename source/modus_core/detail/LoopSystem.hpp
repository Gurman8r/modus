#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/detail/Events.hpp>
#include <modus_core/detail/Memory.hpp>
#include <modus_core/detail/Timer.hpp>

namespace ml
{
	// loop system
	struct ML_CORE_API loop_system : trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type			= typename pmr::polymorphic_allocator<byte_t>;
		using loop_condition			= typename ds::method<bool()>;
		using loop_callback				= typename ds::method<void()>;
		using event_callback			= typename ds::method<void(event const &)>;
		using subsystem					= typename ds::ref<loop_system>;
		using subsystem_list			= typename ds::list<subsystem>;
		using iterator					= typename subsystem_list::iterator;
		using const_iterator			= typename subsystem_list::const_iterator;
		using reverse_iterator			= typename subsystem_list::reverse_iterator;
		using const_reverse_iterator	= typename subsystem_list::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual ~loop_system() noexcept override = default;

		loop_system(event_bus * bus, allocator_type alloc = {}) noexcept
			: event_listener{ bus }
			, m_locked		{}
			, m_uptime		{ false }
			, m_subsystems	{ alloc }
			, m_loopcond	{}
			, m_on_enter	{}
			, m_on_exit		{}
			, m_on_idle		{}
			, m_on_event	{}
		{
		}

		explicit loop_system(loop_system const & other, allocator_type alloc = {})
			: event_listener{ other.get_bus() }
			, m_locked		{}
			, m_uptime		{ false }
			, m_subsystems	{ other.m_subsystems, alloc }
			, m_loopcond	{ other.m_loopcond }
			, m_on_enter	{ other.m_on_enter }
			, m_on_exit		{ other.m_on_exit }
			, m_on_idle		{ other.m_on_idle }
			, m_on_event	{ other.m_on_event }
		{
		}

		explicit loop_system(loop_system && other, allocator_type alloc = {}) noexcept
			: loop_system{ other.get_bus(), alloc }
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
				m_on_event.swap(m_on_event);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using event_listener::subscribe;

		using event_listener::unsubscribe;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool running() const noexcept { return m_locked; }

		ML_NODISCARD auto uptime() const noexcept -> duration { return m_uptime.elapsed(); }

		template <bool Recurse = true
		> int32_t process() noexcept
		{
			// lock
			if (m_locked) { return EXIT_FAILURE * 1; }
			else { m_locked = true; } ML_defer(&) { m_locked = false; };

			// uptime
			m_uptime.restart(); ML_defer(&) { m_uptime.stop(); };

			// enter
			this->run_enter_callback<Recurse>();
			
			// exit
			ML_defer(&) { this->run_exit_callback<Recurse>(); };
			
			// idle
			if (!this->run_loop_condition()) { return EXIT_FAILURE * 2; }
			do { this->run_idle_callback<Recurse>(); }
			while (this->run_loop_condition());
			return EXIT_SUCCESS;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool run_loop_condition() const noexcept
		{
			return m_loopcond && m_loopcond();
		}

		template <bool Recurse = false
		> void run_enter_callback() noexcept
		{
			loop_system::run<Recurse>(&loop_system::m_on_enter, this);
		}

		template <bool Recurse = false
		> void run_exit_callback() noexcept
		{
			loop_system::run<Recurse>(&loop_system::m_on_exit, this);
		}

		template <bool Recurse = false
		> void run_idle_callback() noexcept
		{
			loop_system::run<Recurse>(&loop_system::m_on_idle, this);
		}

		template <bool Recurse = false
		> void run_event_callback(event const & value) noexcept
		{
			loop_system::run<Recurse>(&loop_system::m_on_event, this, ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_loop_condition() const noexcept -> loop_condition const & { return m_loopcond; }

		ML_NODISCARD auto get_enter_callback() const noexcept -> loop_callback const & { return m_on_enter; }

		ML_NODISCARD auto get_exit_callback() const noexcept -> loop_callback const & { return m_on_exit; }

		ML_NODISCARD auto get_idle_callback() const noexcept -> loop_callback const & { return m_on_idle; }

		ML_NODISCARD auto get_event_callback() const noexcept -> event_callback const & { return m_on_event; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Fn, class ... Args
		> auto set_loop_condition(Fn && fn, Args && ... args) noexcept -> loop_condition
		{
			return util::chain(m_loopcond, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_enter_callback(Fn && fn, Args && ... args) -> loop_callback
		{
			return util::chain(m_on_enter, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_exit_callback(Fn && fn, Args && ... args) -> loop_callback
		{
			return util::chain(m_on_exit, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_idle_callback(Fn && fn, Args && ... args) -> loop_callback
		{
			return util::chain(m_on_idle, ML_forward(fn), ML_forward(args)...);
		}

		template <class Fn, class ... Args
		> auto set_event_callback(Fn && fn, Args && ... args) -> event_callback
		{
			return util::chain(m_on_enter, ML_forward(fn), std::placeholders::_1, ML_forward(args)...);
		}

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
		> auto new_subsystem(Args && ... args) noexcept -> ds::ref<Derived>
		{
			static_assert(std::is_base_of_v<loop_system, Derived>, "?");

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

		ML_NODISCARD auto subsystems() & noexcept -> subsystem_list & { return m_subsystems; }

		ML_NODISCARD auto subsystems() const & noexcept -> subsystem_list const & { return m_subsystems; }

		ML_NODISCARD auto subsystems() && noexcept -> subsystem_list && { return std::move(m_subsystems); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		// handle event
		virtual void on_event(event const & value) override
		{
			run_event_callback(value);
		}

		// execute member pointer
		template <bool Recurse = false, class D, class C, class ... Args
		> static void run(D C::*mp, C * self, Args && ... args)
		{
			static_assert(std::is_base_of_v<loop_system, C>, "?");

			if (!self || !mp) { return; } else if (self->*mp)
			{
				std::invoke(self->*mp, ML_forward(args)...);
			}

			if constexpr (Recurse) for (subsystem & e : self->m_subsystems)
			{
				if constexpr (std::is_same_v<C, loop_system>)
				{
					loop_system::run<true>(mp, e.get(), ML_forward(args)...);
				}
				else
				{
					loop_system::run<true>(mp, dynamic_cast<C *>(e.get()), ML_forward(args)...);
				}
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool			m_locked		; // process locked
		timer			m_uptime		; // process timer
		subsystem_list	m_subsystems	; // subsystem list
		loop_condition	m_loopcond		; // loop condition
		loop_callback	m_on_enter		, // enter callback
						m_on_exit		, // exit callback
						m_on_idle		; // idle callback
		event_callback	m_on_event		; // event callback

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_LOOP_SYSTEM_HPP_