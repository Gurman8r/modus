#ifndef _ML_TIMER_HPP_
#define _ML_TIMER_HPP_

#include <modus_core/detail/Duration.hpp>
#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias high_resolution_clock = typename chrono::high_resolution_clock;

	ML_alias time_point = typename high_resolution_clock::time_point;

	struct timer final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		timer() noexcept
			: m_running{}
			, m_start_time{}
			, m_stop_time{}
			, m_elapsed{}
		{
		}

		timer(bool running) noexcept
			: m_running{ running }
			, m_start_time{ high_resolution_clock::now() }
			, m_stop_time{ m_start_time }
			, m_elapsed{}
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
				std::swap(m_elapsed, other.m_elapsed);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool running() const noexcept
		{
			return m_running;
		}

		ML_NODISCARD duration elapsed() const noexcept
		{
			if (m_running)
			{
				return high_resolution_clock::now() - m_start_time;
			}
			else
			{
				return m_elapsed;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		timer & start() noexcept
		{
			return m_running ? (*this) : this->restart();
		}

		timer & restart() noexcept
		{
			m_running = true;

			m_start_time = m_stop_time = high_resolution_clock::now();

			m_elapsed = {};

			return (*this);
		}

		timer & stop() noexcept
		{
			if (!m_running) { return (*this); }

			m_running = false;

			m_elapsed = ((m_stop_time = high_resolution_clock::now()) - m_start_time);

			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool		m_running; // 
		time_point	m_start_time; // 
		time_point	m_stop_time; // 
		duration	m_elapsed; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_TIMER_HPP_