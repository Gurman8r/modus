#ifndef _ML_TYPE_INFO_HPP_
#define _ML_TYPE_INFO_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Based on CTTI:
// https://github.com/Manu343726/ctti
// https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/pretty_function.hpp

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/detail/Hash.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// static string class
#ifndef ML_STATIC_STRING_CLASS
#define ML_STATIC_STRING_CLASS std::basic_string_view<char,struct std::char_traits<char> >
#endif

// static string string
#define ML_STATIC_STRING_STRING ML_stringify(ML_STATIC_STRING_CLASS)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// pretty function
#if defined(ML_cc_msvc)
#	define ML_PRETTY_FUNCTION		__FUNCSIG__
#	define ML_PRETTY_TYPE_PREFIX	"class " ML_STATIC_STRING_STRING " __cdecl ml::pretty_function::type<"
#	define ML_PRETTY_TYPE_SUFFIX	">(void)"
#	define ML_PRETTY_VALUE_PREFIX	"class " ML_STATIC_STRING_STRING " __cdecl ml::pretty_function::value<"
#	define ML_PRETTY_VALUE_DELIM	"; T Value = "
#	define ML_PRETTY_VALUE_SUFFIX	">(void)"
#elif defined(ML_cc_clang)
#	define ML_PRETTY_FUNCTION		__PRETTY_FUNCTION__
#	define ML_PRETTY_TYPE_PREFIX	ML_STATIC_STRING_STRING " ml::pretty_function::type() [T = "
#	define ML_PRETTY_TYPE_SUFFIX	"]"
#	define ML_PRETTY_VALUE_PREFIX	ML_STATIC_STRING_STRING " ml::pretty_function::value() [T = "
#	define ML_PRETTY_VALUE_DELIM	"; Value = "
#	define ML_PRETTY_VALUE_SUFFIX	"]"
#elif defined(ML_cc_gcc)
#	define ML_PRETTY_FUNCTION		__PRETTY_FUNCTION__
#	define ML_PRETTY_TYPE_PREFIX	"constexpr " ML_STATIC_STRING_STRING " ml::pretty_function::type() [with T = "
#	define ML_PRETTY_TYPE_SUFFIX	"]"
#	define ML_PRETTY_VALUE_PREFIX	"constexpr " ML_STATIC_STRING_STRING " ml::pretty_function::value() [with T = "
#	define ML_PRETTY_VALUE_DELIM	"; Value = "
#	define ML_PRETTY_VALUE_SUFFIX	"]"
#else
#	error Type information is not available.
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::pretty_function
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias string = typename ML_STATIC_STRING_CLASS;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T
	> ML_NODISCARD static constexpr string type()
	{
		return { ML_PRETTY_FUNCTION };
	}

	template <class T, T Value
	> ML_NODISCARD static constexpr string value()
	{
		return { ML_PRETTY_FUNCTION };
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ...> struct nameof_t;
	template <class ...> struct typeof;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// name filters
	template <> struct ML_NODISCARD nameof_t<> final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		nameof_t() = delete;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto filter_prefix(pretty_function::string s, pretty_function::string v)
		{
			bool const match
			{
				(s.size() >= v.size() && (s.substr(0, v.size()) == v))
			};
			return match ? s.substr(v.size()) : s;
		}

		static constexpr auto filter_suffix(pretty_function::string s, pretty_function::string v)
		{
			bool const match
			{
				(s.size() >= v.size()) && (s.substr(s.size() - v.size(), v.size()) == v)
			};
			return match ? s.substr(0, (s.size() - v.size())) : s;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		static constexpr auto filter_type(pretty_function::string s) noexcept
		{
			constexpr auto const
				pre{ ML_PRETTY_TYPE_PREFIX },
				suf{ ML_PRETTY_TYPE_SUFFIX };
			return filter_suffix(filter_prefix(s, pre), suf);
		}

		static constexpr auto filter_value(pretty_function::string s) noexcept
		{
			constexpr auto const
				pre{ ML_PRETTY_VALUE_PREFIX },
				dlm{ ML_PRETTY_VALUE_DELIM },
				suf{ ML_PRETTY_VALUE_SUFFIX };
			return s; // NYI
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T> struct nameof_t<T> final
	{
		static constexpr auto value{ nameof_t<>::filter_type(pretty_function::type<T>()) };
	};

#ifdef ML_cc_msvc
	template <> struct nameof_t<int64_t> final
	{
		static constexpr auto value{ "long long" }; // __int64
	};

	template <> struct nameof_t<uint64_t> final
	{
		static constexpr auto value{ "unsigned long long" }; // unsigned __int64
	};
#endif

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// name of type
	template <class T> constexpr auto nameof_v
	{
		pretty_function::string{ nameof_t<T>::value }
	};

	template <class T
	> ML_NODISCARD constexpr auto nameof() noexcept
	{
		return nameof_v<T>;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// hash of type
	template <class T> constexpr hash_t hashof_v
	{
		hashof(pretty_function::string{ nameof_t<T>::value })
	};

	// hash of type
	template <class T
	> ML_NODISCARD constexpr hash_t hashof() noexcept
	{
		return hashof_v<T>;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static_assert("test type info"
		&& nameof_v<bool>			== "bool"
		&& nameof_v<int8_t>			== "signed char"
		&& nameof_v<int16_t>		== "short"
		&& nameof_v<int32_t>		== "int"
		&& nameof_v<int64_t>		== "long long"
		&& nameof_v<uint8_t>		== "unsigned char"
		&& nameof_v<uint16_t>		== "unsigned short"
		&& nameof_v<uint32_t>		== "unsigned int"
		&& nameof_v<uint64_t>		== "unsigned long long"
		&& nameof_v<float32_t>		== "float"
		&& nameof_v<float64_t>		== "double"
		&& nameof_v<float80_t>		== "long double"
		&& nameof_v<char>			== "char"
		&& nameof_v<wchar_t>		== "wchar_t"
		&& nameof_v<char16_t>		== "char16_t"
		&& nameof_v<char32_t>		== "char32_t"
		&& nameof_v<cstring>		== "const char*"
		&& nameof_v<cwstring>		== "const wchar_t*"
		&& nameof_v<c16string>		== "const char16_t*"
		&& nameof_v<c32string>		== "const char32_t*"
#if ML_has_cxx17
		&& nameof_v<std::string>	== "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"
		&& nameof_v<std::wstring>	== "class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >"
		&& nameof_v<std::u16string> == "class std::basic_string<char16_t,struct std::char_traits<char16_t>,class std::allocator<char16_t> >"
		&& nameof_v<std::u32string> == "class std::basic_string<char32_t,struct std::char_traits<char32_t>,class std::allocator<char32_t> >"
#endif
		, "test type info");

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T> struct ML_NODISCARD typeof<T> final
	{
		constexpr typeof() noexcept = default;

		static constexpr auto const & name() noexcept { return nameof_v<T>; }

		static constexpr auto const & hash() noexcept { return hashof_v<T>; }

		template <class ... U
		> constexpr auto compare(typeof<U...> const & other) const noexcept
		{
			return util::compare(m_hash, other.hash());
		}
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct ML_NODISCARD typeof<> final
	{
		constexpr typeof() noexcept
			: m_name{}, m_hash{}
		{
		}

		template <class T
		> constexpr typeof(typeof<T> const & other) noexcept
			: m_name{ other.name() }, m_hash{ other.hash() }
		{
		}

		constexpr auto const & name() const & noexcept { return m_name; }

		constexpr auto const & hash() const & noexcept { return m_hash; }

		constexpr operator hash_t() const noexcept { return this->hash(); }

		template <class ... U
		> constexpr auto compare(typeof<U...> const & other) const noexcept
		{
			return util::compare(m_hash, other.hash());
		}

	private:
		pretty_function::string m_name; hash_t m_hash;
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get type info
	template <class T
	> static constexpr auto typeof_v
	{
		typeof<T>{}
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ... T
	> ML_NODISCARD constexpr bool operator==(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) == 0;
	}

	template <class ... T
	> ML_NODISCARD constexpr bool operator!=(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) != 0;
	}

	template <class ... T
	> ML_NODISCARD constexpr bool operator<(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) < 0;
	}

	template <class ... T
	> ML_NODISCARD constexpr bool operator>(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) > 0;
	}

	template <class ... T
	> ML_NODISCARD constexpr bool operator<=(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) <= 0;
	}

	template <class ... T
	> ML_NODISCARD constexpr bool operator>=(typeof<> const & lhs, typeof<T...> const & rhs) noexcept
	{
		return lhs.compare(rhs) >= 0;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator==(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) == 0;
	}

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator!=(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) != 0;
	}

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator<(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) < 0;
	}

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator>(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) > 0;
	}

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator<=(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) <= 0;
	}

	template <class X, class ... Y
	> ML_NODISCARD constexpr bool operator>=(typeof<X> const & lhs, typeof<Y...> const & rhs) noexcept
	{
		return lhs.compare(rhs) >= 0;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_TYPE_INFO_HPP_