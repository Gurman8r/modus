#ifndef _ML_TIMER_HPP_
#define _ML_TIMER_HPP_

#include <modus_core/detail/Duration.hpp>
#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct timer final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using clock = typename chrono::high_resolution_clock;

		using time_point = typename clock::time_point;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		timer() noexcept
			: m_running		{}
			, m_start_time	{}
			, m_stop_time	{}
		{
		}

		timer(bool running) noexcept
			: m_running		{ running }
			, m_start_time	{ clock::now() }
			, m_stop_time	{ m_start_time }
		{
		}

		timer(timer && other) noexcept : timer{}
		{
			this->swap(std::move(other));
		}

		timer & operator=(timer && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(timer & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_running, other.m_running);
				std::swap(m_start_time, other.m_start_time);
				std::swap(m_stop_time, other.m_stop_time);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool running() const noexcept
		{
			return m_running;
		}

		ML_NODISCARD duration elapsed() const noexcept
		{
			if (m_running) { return clock::now() - m_start_time; }
			else { return m_stop_time - m_start_time; }
		}

		timer & restart() noexcept
		{
			m_running = true;
			m_start_time = clock::now();
			return (*this);
		}

		timer & start() noexcept
		{
			if (m_running) { return (*this); }
			else
			{
				m_running = true;
				m_start_time = clock::now();
				return (*this);
			}
		}

		timer & stop() noexcept
		{
			if (!m_running) { return (*this); }
			else
			{
				m_running = false;
				m_stop_time = clock::now();
				return (*this);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool		m_running		; // 
		time_point	m_start_time	; // 
		time_point	m_stop_time		; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TIMER_HPP_