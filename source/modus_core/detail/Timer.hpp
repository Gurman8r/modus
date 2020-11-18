#ifndef _ML_TIMER_HPP_
#define _ML_TIMER_HPP_

#include <modus_core/detail/Duration.hpp>
#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class _Clock = chrono::high_resolution_clock
	> struct basic_timer final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using clock_type = typename _Clock;

		using self_type = typename basic_timer<clock_type>;

		using time_point = typename clock_type::time_point;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		basic_timer() noexcept
			: m_running		{}
			, m_start_time	{}
			, m_stop_time	{}
			, m_elapsed		{}
		{
		}

		basic_timer(bool running) noexcept
			: m_running		{ running }
			, m_start_time	{ clock_type::now() }
			, m_stop_time	{ m_start_time }
			, m_elapsed		{}
		{
		}

		basic_timer(self_type && other) noexcept : basic_timer{}
		{
			this->swap(std::move(other));
		}

		self_type & operator=(self_type && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(self_type & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_running, other.m_running);
				m_start_time.swap(other.m_start_time);
				m_stop_time.swap(other.m_stop_time);
				m_elapsed.swap(other.m_elapsed);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto elapsed() const noexcept -> duration
		{
			if (m_running)
			{
				return clock_type::now() - m_start_time;
			}
			else
			{
				return m_elapsed;
			}
		}

		ML_NODISCARD bool running() const noexcept
		{
			return m_running;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		auto start() noexcept -> self_type &
		{
			if (!m_running)
			{
				return this->restart();
			}
			else
			{
				return (*this);
			}
		}

		auto restart() noexcept -> self_type &
		{
			m_running = true;
			
			m_start_time = m_stop_time = clock_type::now();
			
			m_elapsed = {};
			
			return (*this);
		}

		auto stop() noexcept -> self_type &
		{
			if (m_running)
			{
				m_running = false;

				m_elapsed = ((m_stop_time = clock_type::now()) - m_start_time);

				return (*this);
			}
			else
			{
				return (*this);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool		m_running		; // 
		time_point	m_start_time	; // 
		time_point	m_stop_time		; // 
		duration	m_elapsed		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias timer = typename basic_timer<>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_TIMER_HPP_