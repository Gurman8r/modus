#ifndef _ML_BITSET_HPP_
#define _ML_BITSET_HPP_

#include <modus_core/detail/Array.hpp>

namespace ml::ds
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t _Count = 64> struct bitset final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr size_t bit_count{ _Count };

		static_assert(0 < bit_count, "bit count negative or zero");

		using self_type = typename _ML ds::bitset<bit_count>;

		using array_type = typename _ML ds::array<bool, bit_count>;

		using value_type = typename std::conditional_t<
			bit_count <= sizeof(uint32) * 8,
			uint32,
			uint64
		>;

		static constexpr ptrdiff_t bits_per_word{ sizeof(value_type) * 8 };
		
		static constexpr ptrdiff_t word_count{ (bit_count - 1) / bits_per_word };

		using storage_type = typename _ML ds::array<value_type, word_count + 1>;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr bitset() noexcept : m_words{} {}

		template <class T, class = std::enable_if_t<std::is_integral_v<T>>
		> constexpr bitset(T const value) noexcept
			: m_words{ static_cast<value_type>(value) }
		{
		}

		constexpr bitset(self_type const & value)
			: m_words{ value.m_words }
		{
		}

		constexpr bitset(self_type && value) noexcept
			: m_words{ std::move(value.m_words) }
		{
		}

		template <size_t N
		> constexpr bitset(bitset<N> const & value)
			: m_words{ value.m_words }
		{
		}

		template <size_t N
		> constexpr bitset(bitset<N> && value) noexcept
			: m_words{ std::move(value.m_words) }
		{
		}

		constexpr bitset(array_type const & value) noexcept
			: m_words{}
		{
			for (auto it = value.begin(); it != value.end(); ++it)
			{
				write(std::distance(str.begin(), it), *it);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr self_type & operator=(self_type const & value)
		{
			self_type temp{ value };
			swap(temp);
			return (*this);
		}

		constexpr self_type & operator=(self_type && value) noexcept
		{
			swap(std::move(value));
			return (*this);
		}

		template <class T, class = std::enable_if_t<std::is_integral_v<T>>
		> constexpr self_type & operator=(T const value) noexcept
		{
			self_type temp{ value };
			swap(temp);
			return (*this);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		constexpr void swap(self_type & value) noexcept
		{
			if (this != std::addressof(value))
			{
				util::swap(m_words, value.m_words);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr operator bool() const noexcept { return m_words; }

		ML_NODISCARD constexpr operator storage_type const & () const & noexcept { return m_words; }

		ML_NODISCARD constexpr bool operator[](size_t const i) const noexcept { return read(i); }

		ML_NODISCARD constexpr auto words() noexcept -> storage_type & { return m_words; }

		ML_NODISCARD constexpr auto words() const noexcept -> storage_type const & { return m_words; }

		ML_NODISCARD constexpr auto size() const noexcept -> size_t { return bit_count; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr bool read(size_t const i) const noexcept
		{
			return ML_bit_read(m_words[i / bits_per_word], i % bits_per_word);
		}

		constexpr bool clear(size_t const i) noexcept
		{
			bool const temp{ this->read(i) };
			
			ML_bit_clear(m_words[i / bits_per_word], i % bits_per_word);
			
			return temp;
		}

		constexpr bool set(size_t const i) noexcept
		{
			bool const temp{ !this->read(i) };
			
			ML_bit_set(m_words[i / bits_per_word], i % bits_per_word);
			
			return temp;
		}

		constexpr bool write(size_t const i, bool const b) noexcept
		{
			return b ? this->set(i) : this->clear(i);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD constexpr array_type arr() const noexcept
		{
			array_type temp{};
			for (size_t i = 0; i < bit_count; ++i)
			{
				temp[i] = read(i);
			}
			return temp;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class Other = self_type
		> constexpr auto compare(Other const & other) const noexcept
		{
			if constexpr (std::is_same_v<Other, self_type>)
			{
				return ML_compare(m_words, other.m_words);
			}
			else
			{
				return this->compare(self_type{ other });
			}
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator==(Other const & other) const
		{
			return this->compare(other) == 0;
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator!=(Other const & other) const
		{
			return this->compare(other) != 0;
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator<(Other const & other) const
		{
			return this->compare(other) < 0;
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator>(Other const & other) const
		{
			return this->compare(other) > 0;
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator<=(Other const & other) const
		{
			return this->compare(other) <= 0;
		}

		template <class Other = self_type
		> ML_NODISCARD constexpr bool operator>=(Other const & other) const
		{
			return this->compare(other) >= 0;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		storage_type m_words;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <size_t N
	> inline std::ostream & operator<<(std::ostream & out, bitset<N> const & value)
	{
		for (size_t i = 0; i < N; ++i)
		{
			out << value.read(i);
		}
		return out;
	}

	template <size_t N
	> inline std::istream & operator>>(std::istream & in, bitset<N> & value)
	{
		for (size_t i = 0; i < N; ++i)
		{
			if (in.good())
			{
				bool b{}; in >> b;
				value.write(i, b);
			}
		}
		return in;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_BITSET_HPP_