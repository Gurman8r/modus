#ifndef _ML_STRING_UTILITY_HPP_
#define _ML_STRING_UTILITY_HPP_

#include <modus_core/detail/String.hpp>

namespace ml::util
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// constexpr strlen
	template <class Ch = char
	> ML_NODISCARD constexpr size_t strlen(Ch const * str)
	{
		return ((*str) ? (1 + util::strlen(str + 1)) : 0);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD constexpr cstring single_str(cstring items_separated_by_zeroes, size_t index)
	{
		cstring	items{ (cstring)items_separated_by_zeroes };
		size_t	count{};
		cstring	ptr{ items };
		while (*ptr)
		{
			if (index == count) { break; }
			ptr += util::strlen(ptr) + 1;
			count++;
		}
		return *ptr ? ptr : "";
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
	> ML_NODISCARD basic_string<Ch> integral_to_string(T const value) noexcept
	{
		// from <string>

		static_assert(std::is_integral_v<T>);

		using U = typename std::make_unsigned_t<T>;

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
					auto chunk{ static_cast<unsigned long>(uval % 1000000000) };
					uval /= 1000000000;
					for (int32 i = 0; i != 9; ++i)
					{
						*--next = static_cast<Ch>('0' + chunk % 10);
						chunk /= 10;
					}
				}
			}
			auto trunc{ static_cast<unsigned long>(uval) };
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
	> ML_NODISCARD basic_string<Ch> floating_point_to_string(T const value) noexcept
	{
		// from <string>

		static_assert(std::is_floating_point_v<T>);
		auto const len{ static_cast<size_t>(::_scprintf("%f", value)) };
		basic_string<Ch> str{ len, 0, pmr::polymorphic_allocator<byte>{} };
		::sprintf_s(str.data(), len + 1, "%f", value);
		return str;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD auto widen(Str const & value) noexcept
	{
		pmr::wstring temp{};
		temp.reserve(value.size());
		for (auto const c : value)
			temp.push_back(static_cast<wchar_t>(c));
		return temp;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD auto narrow(Str const & value) noexcept
	{
		string temp{};
		temp.reserve(value.size());
		for (auto const c : value)
			temp.push_back(static_cast<char>(c));
		return temp;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_NODISCARD inline list<string> tokenize(string value, string const & delim) noexcept
	{
		list<string> temp{};
		auto tok{ std::strtok(value.data(), delim.c_str()) };
		while (tok) {
			temp.push_back(tok);
			tok = std::strtok(nullptr, delim.c_str());
		}
		return temp;
	}

	ML_NODISCARD inline string detokenize(list<string> const & value, string const & delim = " ")
	{
		stringstream ss{};
		for (auto const & str : value) {
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
		default  : return false;
		case '\n':
		case '\t':
		case '\r':
		case ' ' : return true;
		}
	}

	template <class Ch = char
	> struct is_whitespace_t
	{
		ML_NODISCARD constexpr bool operator()(Ch const c) const noexcept {
			return is_whitespace(c);
		}
	};

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str), class Pr = is_whitespace_t<Ch>
	> Str & trim_front(Str & value, Pr pr = {}) noexcept
	{
		while (!value.empty() && pr(value.front()))
		{
			value.erase(value.begin());
		}
		return value;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str), class Pr = is_whitespace_t<Ch>
	> Str & trim_back(Str & value, Pr pr = {}) noexcept
	{
		while (!value.empty() && pr(value.back()))
		{
			value.pop_back();
		}
		return value;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str), class Pr = is_whitespace_t<Ch>
	> ML_NODISCARD Str & trim(Str & value, Pr pr = {}) noexcept
	{
		return trim_front(trim_back(value, pr), pr);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
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

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str remove_all(Str value, Str const & toRemove) noexcept
	{
		return replace_all(value, toRemove, {});
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_alpha(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isalpha(elem, {}))
				return false;
		return true;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_alnum(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isalnum(elem, {}))
				return false;
		return true;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_graph(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isgraph(elem, {}))
				return false;
		return true;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_print(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::isprint(elem, {}))
				return false;
		return true;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_lower(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::islower(elem, {}))
				return false;
		return true;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_upper(Str const & value) noexcept
	{
		if (value.empty()) return false;
		for (auto const & elem : value)
			if (!std::islower(elem, {}))
				return false;
		return true;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_lower(Str value) noexcept
	{
		for (auto & elem : value)
			elem = std::tolower(elem, {});
		return value;
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_upper(Str value) noexcept
	{
		for (auto & elem : value)
			elem = std::toupper(elem, {});
		return value;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
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

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_integer(Str const & value) noexcept
	{
		if (value.empty()) return false;
		auto it{ std::begin(value) };
		if ((*it) == '-') it++; 
		while (it != std::end(value) && std::isdigit(*it, {})) ++it;
		return (it == std::end(value));
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD bool is_decimal(Str const & value) noexcept
	{
		if (value.empty()) return false;
		char * endptr{ nullptr };
		auto temp { std::strtod(value.c_str(), &endptr) };
		return !(*endptr != 0 || endptr == value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
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

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
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

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int8> to_i8(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<int8>(ML_forward(str).c_str(), &::strtol, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int16> to_i16(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<int16>(ML_forward(str).c_str(), &::strtol, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int32> to_i32(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<int32>(ML_forward(str).c_str(), &::strtol, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<int64> to_i64(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<int64>(ML_forward(str).c_str(), &::strtoll, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint8> to_u8(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<uint8>(ML_forward(str).c_str(), &::strtoul, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint16> to_u16(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<uint16>(ML_forward(str).c_str(), &::strtoul, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint32> to_u32(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<uint32>(ML_forward(str).c_str(), &::strtoul, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<uint64> to_u64(Str && str, int32 base = 10) noexcept
	{
		return util::parse_answer<uint64>(ML_forward(str).c_str(), &::strtoull, base);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float32> to_f32(Str && str) noexcept
	{
		return util::parse_answer<float32>(ML_forward(str).c_str(), &::strtod);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float64> to_f64(Str && str) noexcept
	{
		return util::parse_answer<float64>(ML_forward(str).c_str(), &::strtod);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD std::optional<float80> to_f80(Str && str) noexcept
	{
		return util::parse_answer<float80>(ML_forward(str).c_str(), &::strtold);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int8 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int16 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int32 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(int64 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint8 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint16 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint32 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(uint64 const value) noexcept
	{
		return util::integral_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float64 const value) noexcept
	{
		return util::floating_point_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float32 const value) noexcept
	{
		return util::floating_point_to_string<Ch>(value);
	}

	template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(float80 const value) noexcept
	{
		return util::floating_point_to_string<Ch>(value);
	}

	template <class T, ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
	> ML_NODISCARD Str to_string(T && value) noexcept
	{
		std::basic_stringstream<Ch, Tr, Al> ss{};
		ss << ML_forward(value);
		return ss.str();
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Arg0, class ... Args
	> ML_NODISCARD string format(string const & str, Arg0 const & arg0, Args && ... args) noexcept
	{
		stringstream ss{};
		ss << ML_forward(arg0) << '\n';
		int32 sink[] = { 0, ((void)(ss << args << '\n'), 0)... }; (void)sink;
		return format(str, ss);
	}

	ML_NODISCARD inline string format(string str, stringstream & ss) noexcept
	{
		for (size_t i = 0; ss.good(); ++i)
		{
			if (string line; std::getline(ss, line))
			{
				string const fmt{ "{" + to_string(i) + "}" };

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
	> ML_NODISCARD string format(string str, list<Str> const & values) noexcept
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			string const fmt{ "{" + to_string(i) + "}" };

			for (size_t j = 0; (j = str.find(fmt, j)) != str.npos;)
			{
				str.replace(j, fmt.size(), values[i]);

				j += values[i].size();
			}
		}
		return str;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Ch = char
	> int32 textv(Ch * buf, size_t size, Ch const * fmt, va_list args)
	{
		int32 w;
		if constexpr (_ML util::is_narrow_v<Ch>)
		{
			w = std::vsnprintf((char *)buf, size, (cstring)fmt, args);
		}
		else
		{
			w = std::vswprintf((wchar_t *)buf, size, (cwstring)fmt, args);
		}
		if (!buf) { return w; }
		if (w == -1 || w >= (int32)size) { w = (int32)size - 1; }
		buf[w] = 0;
		return w;
	}

	template <class Ch, size_t N
	> int32 textv(Ch(&buf)[N], Ch const * fmt, va_list args)
	{
		return _ML util::textv(buf, N, fmt, args);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_STRING_UTILITY_HPP_