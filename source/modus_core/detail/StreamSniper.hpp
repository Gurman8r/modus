#ifndef _ML_STREAM_SNIPER_HPP_
#define _ML_STREAM_SNIPER_HPP_

#include <modus_core/detail/String.hpp>

namespace ml
{
	template <ML_STRING_TEMPLATE(Ch, Tr, Al)
	> struct basic_stream_sniper final : non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using char_type			= typename Ch;
		using traits_type		= typename Tr;
		using allocator_type	= typename Al;
		using self_type			= typename basic_stream_sniper<Ch, Tr, Al>;
		using string_type		= typename std::basic_string<Ch, Tr, Al>;
		using stringbuf_type	= typename std::basic_stringbuf<Ch, Tr, Al>;
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
				std::swap(m_cur	, value.m_cur);
				std::swap(m_old	, value.m_old);
				std::swap(m_ss	, value.m_ss);
			}
		}

		void update(ostream_type * value) noexcept
		{
			if (value && !m_cur && !m_old)
			{
				m_old = value;
				m_cur = m_old->rdbuf(m_ss.rdbuf());
				m_ss.str({});
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

		ML_NODISCARD auto sstr() & noexcept -> sstream_type & { return m_ss; }

		ML_NODISCARD auto sstr() const & noexcept -> sstream_type const & { return m_ss; }

		ML_NODISCARD auto sstr() && noexcept -> sstream_type && { return std::move(m_ss); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto rdbuf() const noexcept -> stringbuf_type * { return m_ss.rdbuf(); }

		ML_NODISCARD auto str() const noexcept -> string_type { return m_ss.str(); }

		template <class Value = string_type
		> ML_NODISCARD void str(Value && value) noexcept { m_ss.str(ML_forward(value)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		streambuf_type *	m_cur	{}; // current stream
		ostream_type *		m_old	{}; // previous stream
		sstream_type		m_ss	{}; // captured data

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	ML_alias stream_sniper = typename basic_stream_sniper<char>;

	ML_alias wstream_sniper = typename basic_stream_sniper<wchar_t>;
}

#endif // !_ML_STREAM_SNIPER_HPP_