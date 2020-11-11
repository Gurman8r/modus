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

		using clock_type	= typename _Clock;
		using self_type		= typename basic_timer<clock_type>;
		using time_point	= typename clock_type::time_point;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		basic_timer(bool running = true) noexcept
			: m_running		{ running }
			, m_start_time	{ clock_type::now() }
			, m_stop_time	{ m_start_time }
			, m_elapsed		{}
		{
		}

		basic_timer(self_type && other) noexcept
		{
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
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool running() const noexcept
		{
			return m_running;
		}

		ML_NODISCARD duration elapsed() const noexcept
		{
			return m_running ? (clock_type::now() - m_start_time) : m_elapsed;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		self_type & start() & noexcept
		{
			if (!m_running)
			{
				this->restart();
			}
			return (*this);
		}

		self_type & restart() & noexcept
		{
			m_running = true;
			
			m_start_time = m_stop_time = clock_type::now();
			
			m_elapsed = {};
			
			return (*this);
		}

		self_type & stop() & noexcept
		{
			if (m_running)
			{
				m_running = false;
				
				m_elapsed = ((m_stop_time = clock_type::now()) - m_start_time);
			}
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		time_point	m_start_time; // 
		time_point	m_stop_time	; // 
		duration	m_elapsed	; // 
		bool		m_running	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias timer = typename basic_timer<>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_TIMER_HPP_