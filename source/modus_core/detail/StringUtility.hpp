#ifndef _ML_STRING_UTILITY_HPP_
#define _ML_STRING_UTILITY_HPP_

#include <detail/Utility.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// pmr string streams
namespace std::pmr
{
	using stringstream = basic_stringstream<char, char_traits<char>, polymorphic_allocator<char>>;
	using wstringstream = basic_stringstream<wchar_t, char_traits<wchar_t>, polymorphic_allocator<wchar_t>>;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// std string template
#define ML_STD_STRING_TEMPLATE(Ch, Tr, Al, St)	\
	class Ch = char,							\
	class Tr = std::char_traits<Ch>,			\
	class Al = std::allocator<Ch>,				\
	class St = std::basic_string<Ch, Tr, Al>

// pmr string template
#define ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, St)	\
	class Ch = char,							\
	class Tr = std::char_traits<Ch>,			\
	class Al = pmr::polymorphic_allocator<Ch>,	\
	class St = pmr::basic_string<Ch, Tr>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// STRING TRAITS
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// is character
	template <class Ch
	> static constexpr bool is_char_v
	{
		_ML util::is_any_of_v<Ch, char, wchar_t, char16_t, char32_t>
	};

	template <class Ch
	> struct is_char_t : std::bool_constant<is_char_v<Ch>> {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// is character pointer
	template <class T, class Ch = char
	> static constexpr bool is_cstring_v
	{
		_ML util::is_char_v<Ch> &&
		std::is_convertible_v<T const &, Ch const *>
	};

	template <class T, class Ch = char
	> struct is_cstring_t : std::bool_constant<is_cstring_v<T, Ch>> {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// is string view
	template <class T, class Ch = char, class Traits = std::char_traits<Ch>
	> static constexpr bool is_string_view_v
	{
		_ML util::is_cstring_v<T, Ch> ||
		std::is_convertible_v<T const &, std::basic_string_view<Ch, Traits>>
	};

	template <class T, class Ch = char, class Traits = std::char_traits<Ch>
	> struct is_string_view_t : std::bool_constant<is_string_view_v<T, Ch, Traits>> {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// is string
	template <class T, class Ch = char
	> static constexpr bool is_string_v
	{
		_ML util::is_any_of_v<T, std::basic_string<Ch>, pmr::basic_string<Ch>>
	};

	template <class T, class Ch = char
	> struct is_string_t : std::bool_constant<is_string_v<T, Ch>> {};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// STRING FUNCTIONS
namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// constexpr strlen
	template <class Ch = char
	> ML_NODISCARD constexpr size_t strlen(Ch const * str)
	{
		return ((*str) ? (1 + _ML util::strlen(str + 1)) : 0);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T, class Ch, class Fn, class ... Args
	> ML_NODISCARD std::optional<T> parse_answer(Ch const * ptr, Fn && fn, Args && ... args) noexcept
	{
		// from <string>

		Ch * end{};
		auto const answer{ std::invoke(ML_forward(fn), ptr, &end, ML_forward(args)...) };
		if (!(*end != 0 || end == ptr))
		{
			return static_cast<T>(answer);
		}
		else
		{
			return std::nullopt;
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Ch, class T
	> ML_NODISCARD pmr::basic_string<Ch> integral_to_string(T const value) noexcept
	{
		// from <string>

		static_assert(std::is_integral_v<T>);

		using		U		= typename std::make_unsigned_t<T>;
		Ch			buf[21]	{};
		Ch * const	end		{ std::end(buf) };
		Ch *		next	{ end };
		auto const	uval	{ static_cast<U>(value) };

		auto uint_to_string = [](Ch * next, auto uval) noexcept
		{
			if constexpr (sizeof(U) > 4)
			{
				while (uval > 0xFFFFFFFFU)
				{
					auto chunk{ static_cast<ulong_t>(uval % 1000000000) };
					uval /= 1000000000;
					for (int32_t i = 0; i != 9; ++i)
					{
						*--next = static_cast<Ch>('0' + chunk % 10);
						chunk /= 10;
					}
				}
			}
			auto trunc{ static_cast<ulong_t>(uval) };
			do {
				*--next = static_cast<Ch>('0' + trunc % 10);
				trunc /= 10;
			} while (trunc != 0);
			return next;
		};

		if (value < 0)
		{
			next = uint_to_string(next, 0 - uval);
			*--next = '-';
		}
		else
		{
			next = uint_to_string(next, uval);
		}
		return { next, end };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Ch, class T
	> ML_NODISCARD pmr::basic_string<Ch> floating_point_to_string(T const value) noexcept
	{
		// from <string>

		static_assert(std::is_floating_point_v<T>);
		auto const len{ static_cast<size_t>(_CSTD _scprintf("%f", value)) };
		pmr::basic_string<Ch> str{ len, 0, pmr::polymorphic_allocator<byte_t>{} };
		_CSTD sprintf_s(str.data(), len + 1, "%f", value);
		return str;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD auto widen(Str const & value) noexcept
	{
		pmr::wstring temp{};
		temp.reserve(value.size());
		for (auto const c : value)
			temp.push_back(static_cast<wchar_t>(c));
		return temp;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD auto narrow(Str const & value) noexcept
	{
		pmr::string temp{};
		temp.reserve(value.size());
		for (auto const c : value)
			temp.push_back(static_cast<char>(c));
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD inline pmr::vector<pmr::string> tokenize(pmr::string value, pmr::string const & delim) noexcept
	{
		if (value.empty()) { return {}; }
		if (delim.empty()) { return { value }; }
		pmr::vector<pmr::string> temp{};
		size_t i{};
		while ((i = value.find(delim)) != value.npos)
		{
			temp.push_back(value.substr(0, i));
			value.erase(0, i + delim.size());
		}
		temp.push_back(value);
		return temp;
	}

	ML_NODISCARD inline pmr::string detokenize(pmr::vector<pmr::string> const & value, pmr::string const & delim = " ")
	{
		if (value.empty()) { return {}; }
		if (value.size() == 1) { return value.front(); }
		pmr::stringstream ss{};
		for (auto const & str : value)
		{
			ss << str << delim;
		}
		return ss.str();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Ch = char
	> ML_NODISCARD constexpr bool is_whitespace(Ch const c) noexcept
	{
		switch (static_cast<char>(c))
		{
		case '\n':
		case '\t':
		case '\r':
		case ' ' : return true;
		default  : return false;
		}
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str trim_front(Str value) noexcept
	{
		while (!value.empty() && is_whitespace(value.front()))
		{
			value.erase(value.begin());
		}
		return value;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str trim_back(Str value) noexcept
	{
		while (!value.empty() && is_whitespace(value.back()))
		{
			value.pop_back();
		}
		return value;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str trim(Str value) noexcept
	{
		return trim_front(trim_back(value));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str replace_all(Str value, Str const & toFind, Str const & replaceWith) noexcept
	{
		if (!value.empty() && !toFind.empty())
		{
			for (size_t i = 0; (i = value.find(toFind, i)) != Str::npos;)
			{
				value.replace(i, toFind.size(), replaceWith);
				i += replaceWith.size();
			}
		}
		return value;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str remove_all(Str value, Str const & toRemove) noexcept
	{
		return replace_all(value, toRemove, {});
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_alpha(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isalpha(elem, {}))
				return false;
		return true;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_alnum(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isalnum(elem, {}))
				return false;
		return true;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_graph(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isgraph(elem, {}))
				return false;
		return true;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_print(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isprint(elem, {}))
				return false;
		return true;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_lower(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::islower(elem, {}))
				return false;
		return true;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_upper(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::islower(elem, {}))
				return false;
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_lower(Str value) noexcept
	{
		for (auto & elem : value)
			elem = std::tolower(elem, {});
		return value;
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_upper(Str value) noexcept
	{
		for (auto & elem : value)
			elem = std::toupper(elem, {});
		return value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_bool(Str const & value) noexcept
	{
		switch (hashof(to_lower(value)))
		{
		case hashof("1"):
		case hashof("true"):
		case hashof("on"):
		case hashof("yes"):
		case hashof("0"):
		case hashof("false"):
		case hashof("off"):
		case hashof("no"):
			return true;
		default:
			return false;
		}
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_integer(Str const & value) noexcept
	{
		if (value.empty()) return false;
		auto it{ std::begin(value) };
		if ((*it) == '-') it++; 
		while (it != std::end(value) && std::isdigit(*it, {})) ++it;
		return (it == std::end(value));
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_decimal(Str const & value) noexcept
	{
		if (value.empty()) return false;
		char * endptr{ nullptr };
		auto temp { std::strtod(value.c_str(), &endptr) };
		return !(*endptr != 0 || endptr == value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_name(Str const & value) noexcept
	{
		auto first{ std::begin(value) }, last{ std::end(value) };
		if (0 == std::distance(first, last)) { return false; }
		if (!(*first == '_') || !std::isalpha(*first, {})) { return false; }
		++first;
		for (; first != last; ++first)
		{
			if ((*first != '_') || !std::isalnum(*first, {}))
			{
				return false;
			}
		}
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<bool> to_bool(Str && str) noexcept
	{
		switch (hashof(to_lower(ML_forward(str))))
		{
		case hashof("1"):
		case hashof("true"):
		case hashof("on"):
		case hashof("yes"):
			return std::make_optional(true);
		case hashof("0"):
		case hashof("false"):
		case hashof("off"):
		case hashof("no"):
			return std::make_optional(false);
		default:
			return std::nullopt;
		}
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int8_t> to_i8(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<int8_t>(ML_forward(str).c_str(), &_CSTD strtol, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int16_t> to_i16(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<int16_t>(ML_forward(str).c_str(), &_CSTD strtol, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int32_t> to_i32(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<int32_t>(ML_forward(str).c_str(), &_CSTD strtol, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int64_t> to_i64(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<int64_t>(ML_forward(str).c_str(), &_CSTD strtoll, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint8_t> to_u8(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<uint8_t>(ML_forward(str).c_str(), &_CSTD strtoul, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint16_t> to_u16(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<uint16_t>(ML_forward(str).c_str(), &_CSTD strtoul, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint32_t> to_u32(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<uint32_t>(ML_forward(str).c_str(), &_CSTD strtoul, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint64_t> to_u64(Str && str, int32_t base = 10) noexcept
	{
		return _ML util::parse_answer<uint64_t>(ML_forward(str).c_str(), &_CSTD strtoull, base);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float32_t> to_f32(Str && str) noexcept
	{
		return _ML util::parse_answer<float32_t>(ML_forward(str).c_str(), &_CSTD strtod);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float64_t> to_f64(Str && str) noexcept
	{
		return _ML util::parse_answer<float64_t>(ML_forward(str).c_str(), &_CSTD strtod);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float80_t> to_f80(Str && str) noexcept
	{
		return _ML util::parse_answer<float80_t>(ML_forward(str).c_str(), &_CSTD strtold);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int8_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int16_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int32_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int64_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint8_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint16_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint32_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint64_t const value) noexcept
	{
		return _ML util::integral_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float64_t const value) noexcept
	{
		return _ML util::floating_point_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float32_t const value) noexcept
	{
		return _ML util::floating_point_to_string<Ch>(value);
	}

	template <ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float80_t const value) noexcept
	{
		return _ML util::floating_point_to_string<Ch>(value);
	}

	template <class T, ML_PMR_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(T && value) noexcept
	{
		std::basic_stringstream<Ch, Tr, Al> ss{};
		ss << ML_forward(value);
		return ss.str();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Arg0, class ... Args
	> ML_NODISCARD pmr::string format(pmr::string const & str, Arg0 const & arg0, Args && ... args) noexcept
	{
		pmr::stringstream ss{};
		ss << ML_forward(arg0) << '\n';
		int32_t sink[] = { 0, ((void)(ss << args << '\n'), 0)... }; (void)sink;
		return format(str, ss);
	}

	ML_NODISCARD inline pmr::string format(pmr::string str, pmr::stringstream & ss) noexcept
	{
		for (size_t i = 0; ss.good(); ++i)
		{
			if (pmr::string line; std::getline(ss, line))
			{
				pmr::string const fmt{ "{" + to_string(i) + "}" };

				for (size_t j = 0; (j = str.find(fmt, j)) != str.npos;)
				{
					str.replace(j, fmt.size(), line);

					j += line.size();
				}
			}
		}
		return str;
	}

	template <class Str
	> ML_NODISCARD pmr::string format(pmr::string str, pmr::vector<Str> const & values) noexcept
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			pmr::string const fmt{ "{" + to_string(i) + "}" };

			for (size_t j = 0; (j = str.find(fmt, j)) != str.npos;)
			{
				str.replace(j, fmt.size(), values[i]);

				j += values[i].size();
			}
		}
		return str;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_STRING_UTILITY_HPP_