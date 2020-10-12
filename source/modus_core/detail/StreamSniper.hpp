#ifndef _ML_STREAM_SNIPER_HPP_
#define _ML_STREAM_SNIPER_HPP_

#include <modus_core/detail/NonCopyable.hpp>

namespace ml
{
	template <
		class Ch = char,
		class Tr = std::char_traits<Ch>,
		class Al = pmr::polymorphic_allocator<Ch>
	> struct basic_stream_sniper final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using char_type			= typename Ch;
		using traits_type		= typename Tr;
		using allocator_type	= typename Al;
		using self_type			= typename basic_stream_sniper<Ch, Tr, Al>;
		using sstream_type		= typename std::basic_stringstream<Ch, Tr, Al>;
		using ostream_type		= typename std::basic_ostream<Ch, Tr>;
		using streambuf_type	= typename std::basic_streambuf<Ch, Tr>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		basic_stream_sniper(ostream_type * value = {}) noexcept
		{
			this->update(value);
		}

		basic_stream_sniper(self_type && value) noexcept
		{
			this->swap(std::move(value));
		}

		~basic_stream_sniper() noexcept
		{
			this->update(nullptr);
		}

		basic_stream_sniper & operator=(self_type && value) noexcept
		{
			this->swap(std::move(value));
			return (*this);
		}

		void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				std::swap(m_cur, value.m_cur);
				std::swap(m_old, value.m_old);
				std::swap(m_str, value.m_str);
			}
		}

		void update(ostream_type * value) noexcept
		{
			if (value && !m_cur && !m_old)
			{
				m_old = value;
				m_cur = m_old->rdbuf(m_str.rdbuf());
				m_str.str({});
			}
			else if (!value && m_cur && m_old)
			{
				m_old->rdbuf(m_cur);
				m_old = nullptr;
				m_cur = nullptr;
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD operator bool() const noexcept { return (m_cur && m_old); }

		ML_NODISCARD auto sstr() & noexcept -> sstream_type & { return m_str; }

		ML_NODISCARD auto sstr() const & noexcept -> sstream_type const & { return m_str; }

		ML_NODISCARD auto sstr() && noexcept -> sstream_type && { return std::move(m_str); }

		ML_NODISCARD auto sstr() const && noexcept -> sstream_type const && { return std::move(m_str); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		streambuf_type *	m_cur{}; // current stream
		ostream_type *		m_old{}; // previous stream
		sstream_type		m_str{}; // captured data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_STREAM_SNIPER_HPP_