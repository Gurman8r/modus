#ifndef _ML_LOOP_SYSTEM_HPP_
#define _ML_LOOP_SYSTEM_HPP_

#include <modus_core/runtime/Runtime.hpp>

namespace ml
{
	// loop system
	struct ML_CORE_API loop_system : runtime_listener<loop_system>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using loop_condition			= typename ds::method<bool()>;
		using subsystem					= typename ds::ref<loop_system>;
		using subsystem_list			= typename ds::list<subsystem>;
		using iterator					= typename subsystem_list::iterator;
		using const_iterator			= typename subsystem_list::const_iterator;
		using reverse_iterator			= typename subsystem_list::reverse_iterator;
		using const_reverse_iterator	= typename subsystem_list::const_reverse_iterator;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		loop_system(runtime_api * api, loop_condition const & loopcond = {}) noexcept;

		virtual ~loop_system() noexcept override;
		
		template <class Fn, class Arg0, class ... Args
		> loop_system(runtime_api * api, Fn && fn, Arg0 && arg0, Args && ... args) noexcept
			: loop_system{ api, std::bind(ML_forward(fn), ML_forward(arg0), ML_forward(args)...) }
		{
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process() noexcept
		{
			if (!lock()) { return EXIT_FAILURE * 1; } ML_defer(&) { unlock(); };

			on_process_enter(); ML_defer(&) { on_process_exit(); };

			if (!test_condition()) { return EXIT_FAILURE * 2; }

			do { on_process_idle(); } while (test_condition());

			return EXIT_SUCCESS;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_condition() const noexcept -> loop_condition const &
		{
			return m_loopcond;
		}

		template <class Fn, class ... Args
		> auto set_condition(Fn && fn, Args && ... args) noexcept -> loop_condition &
		{
			if constexpr (0 == sizeof...(args))
			{
				return m_loopcond = ML_forward(fn);
			}
			else
			{
				return m_loopcond = std::bind(ML_forward(fn), ML_forward(args)...);
			}
		}

		ML_NODISCARD bool test_condition() const noexcept
		{
			return m_loopcond && m_loopcond();
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool is_valid_subsystem(subsystem const & value) const noexcept
		{
			return value
				&& this != value.get()
				&& get_api() == value->get_api();
		}

		bool contains(subsystem const & value) const noexcept
		{
			return is_valid_subsystem(value) && (find(value) != end());
		}

		template <class Derived = loop_system, class ... Args
		> auto emplace(Args && ... args) noexcept -> ds::ref<Derived>
		{
			static_assert(std::is_base_of_v<loop_system, Derived>, "?");

			return std::static_pointer_cast<Derived>(m_subsystems.emplace_back
			(
				get_memory()->make_ref<Derived>(get_api(), ML_forward(args)...)
			));
		}

		auto erase(const_iterator it) -> iterator
		{
			return (it == end()) ? end() : m_subsystems.erase(it);
		}

		auto erase(subsystem const & value) noexcept -> iterator
		{
			return erase(find(value));
		}

		ML_NODISCARD auto find(subsystem const & value) noexcept -> iterator
		{
			return is_valid_subsystem(value) ? std::find(begin(), end(), value) : end();
		}

		ML_NODISCARD auto find(subsystem const & value) const noexcept -> const_iterator
		{
			return is_valid_subsystem(value) ? std::find(begin(), end(), value) : end();
		}

		auto insert(subsystem const & value) noexcept -> subsystem
		{
			if (!is_valid_subsystem(value))
			{
				return nullptr;
			}
			else if (auto const it{ find(value) }; it != end())
			{
				return (*it);
			}
			else
			{
				return m_subsystems.emplace_back(value);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void reserve(size_t capacity) { m_subsystems.reserve(capacity); }

		void resize(size_t capacity) { m_subsystems.resize(capacity); }

		void shrink_to_fit() { m_subsystems.shrink_to_fit(); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto operator[](size_t const i) noexcept -> subsystem & { return m_subsystems[i]; }

		ML_NODISCARD auto operator[](size_t const i) const noexcept -> subsystem const & { return m_subsystems[i]; }

		ML_NODISCARD auto at(size_t const i) noexcept -> subsystem & { return m_subsystems.at(i); }

		ML_NODISCARD auto at(size_t const i) const noexcept -> subsystem const & { return m_subsystems.at(i); }

		ML_NODISCARD auto back() noexcept -> subsystem & { return m_subsystems.back(); }

		ML_NODISCARD auto back() const noexcept -> subsystem const & { return m_subsystems.back(); }

		ML_NODISCARD bool empty() const noexcept { return m_subsystems.empty(); }

		ML_NODISCARD auto data() noexcept -> subsystem * { return m_subsystems.data(); }
		
		ML_NODISCARD auto data() const noexcept -> subsystem const * { return m_subsystems.data(); }

		ML_NODISCARD auto front() noexcept -> subsystem & { return m_subsystems.front(); }

		ML_NODISCARD auto front() const noexcept -> subsystem const & { return m_subsystems.front(); }
		
		ML_NODISCARD auto size() const noexcept -> size_t { return m_subsystems.size(); }
		
		ML_NODISCARD auto max_size() const noexcept -> size_t { return m_subsystems.max_size(); }

		ML_NODISCARD auto subsystems() noexcept -> subsystem_list & { return m_subsystems; }

		ML_NODISCARD auto subsystems() const noexcept -> subsystem_list const & { return m_subsystems; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator subsystem_list & () noexcept { return m_subsystems; }

		ML_NODISCARD operator subsystem_list const & () const noexcept { return m_subsystems; }

		ML_NODISCARD operator subsystem_list && () noexcept { return std::move(m_subsystems); }

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
		bool lock() noexcept { return !m_locked && (m_locked = true); }

		bool unlock() noexcept { return m_locked && !(m_locked = false); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_process_enter() { for (auto & e : *this) { e->on_process_enter(); } }

		virtual void on_process_exit() { for (auto & e : *this) { e->on_process_exit(); } }

		virtual void on_process_idle() { for (auto & e : *this) { e->on_process_idle(); } }

		virtual void on_event(event && ev) override { for (auto & e : *this) { e->on_event(ML_forward(ev)); } }

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