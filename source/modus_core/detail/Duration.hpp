#ifndef _ML_DURATION_HPP_
#define _ML_DURATION_HPP_

#include <modus_core/detail/Utility.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias nanoseconds_t	= typename chrono::duration<float32, std::nano>;
	ML_alias microseconds_t	= typename chrono::duration<float32, std::micro>;
	ML_alias milliseconds_t	= typename chrono::duration<float32, std::milli>;
	ML_alias seconds_t		= typename chrono::duration<float32, std::ratio<1>>;
	ML_alias minutes_t		= typename chrono::duration<float32, std::ratio<60>>;
	ML_alias hours_t		= typename chrono::duration<float32, std::ratio<60 * 60>>;
	ML_alias days_t			= typename chrono::duration<float32, std::ratio<60 * 60 * 24>>;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	struct duration final : public chrono::duration<float32>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using chrono::duration<float32>::duration;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr nanoseconds_t nanoseconds() const noexcept
		{
			return chrono::duration_cast<nanoseconds_t>(*this);
		}

		ML_NODISCARD constexpr microseconds_t microseconds() const noexcept
		{
			return chrono::duration_cast<microseconds_t>(*this);
		}

		ML_NODISCARD constexpr milliseconds_t milliseconds() const noexcept
		{
			return chrono::duration_cast<milliseconds_t>(*this);
		}

		ML_NODISCARD constexpr seconds_t seconds() const noexcept
		{
			return chrono::duration_cast<seconds_t>(*this);
		}

		ML_NODISCARD constexpr minutes_t minutes() const noexcept
		{
			return chrono::duration_cast<minutes_t>(*this);
		}

		ML_NODISCARD constexpr hours_t hours() const noexcept
		{
			return chrono::duration_cast<hours_t>(*this);
		}

		ML_NODISCARD constexpr days_t days() const noexcept
		{
			return chrono::duration_cast<days_t>(*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	static void to_json(json & j, duration const & v)
	{
		j = v.count();
	}

	static void from_json(json const & j, duration & v)
	{
		j.get_to((float32 &)v);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DURATION_HPP_