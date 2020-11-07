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

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias static_string = typename ML_STATIC_STRING_CLASS;
	
	namespace pretty_function
	{
		template <class T
		> ML_NODISCARD static constexpr static_string type()
		{
			return { ML_PRETTY_FUNCTION };
		}

		template <class T, T Value
		> ML_NODISCARD static constexpr static_string value()
		{
			return { ML_PRETTY_FUNCTION };
		}
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ...> struct nameof_t;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// name filters
	template <> struct nameof_t<> final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		nameof_t() = delete;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static constexpr auto filter_prefix(static_string s, static_string v)
		{
			bool const match
			{
				(s.size() >= v.size() && (s.substr(0, v.size()) == v))
			};
			return match ? s.substr(v.size()) : s;
		}

		ML_NODISCARD static constexpr auto filter_suffix(static_string s, static_string v)
		{
			bool const match
			{
				(s.size() >= v.size()) && (s.substr(s.size() - v.size(), v.size()) == v)
			};
			return match ? s.substr(0, (s.size() - v.size())) : s;
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static constexpr auto filter_type(static_string s) noexcept
		{
			constexpr auto const
				pre{ ML_PRETTY_TYPE_PREFIX },
				suf{ ML_PRETTY_TYPE_SUFFIX };
			return filter_suffix(filter_prefix(s, pre), suf);
		}

		ML_NODISCARD static constexpr auto filter_value(static_string s) noexcept
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

	template <class T
	> ML_NODISCARD constexpr static_string nameof() noexcept
	{
		return nameof_t<T>::value;
	}

	template <class T
	> constexpr static_string nameof_v
	{
		nameof<T>()
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// hash of type

	template <class T
	> ML_NODISCARD constexpr hash_t hashof() noexcept
	{
		return hashof(static_string{ nameof_t<T>::value });
	}

	template <class T
	> constexpr hash_t hashof_v
	{
		hashof<T>()
	};

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

	template <class ...> struct typeof;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class T> struct typeof<T> final
	{
		constexpr typeof() noexcept = default;

		ML_NODISCARD constexpr operator hash_t const & () const & noexcept { return this->hash(); }

		ML_NODISCARD static constexpr hash_t const & hash() noexcept { return hashof_v<T>; }

		ML_NODISCARD static constexpr static_string const & name() noexcept { return nameof_v<T>; }
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <> struct typeof<> final
	{
		constexpr typeof() noexcept
			: m_name{}, m_hash{}
		{
		}

		template <class ... T
		> constexpr typeof(typeof<T...> const & other) noexcept
			: m_hash{ other.hash() }, m_name{ other.name() }
		{
		}

		ML_NODISCARD constexpr operator hash_t const & () const & noexcept { return this->hash(); }

		ML_NODISCARD constexpr hash_t const & hash() const & noexcept { return m_hash; }

		ML_NODISCARD constexpr static_string const & name() const & noexcept { return m_name; }

	private:
		hash_t			m_hash	; // hash code
		static_string	m_name	; // type name
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// get type

	template <class T
	> static constexpr auto typeof_v
	{
		typeof<T>{}
	};
	
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator==(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs == (hash_t)rhs;
	}

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator!=(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs != (hash_t)rhs;
	}

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator<(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs < (hash_t)rhs;
	}

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator>(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs > (hash_t)rhs;
	}

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator<=(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs <= (hash_t)rhs;
	}

	template <class ... L, class ... R
	> ML_NODISCARD constexpr bool operator>=(typeof<L...> const & lhs, typeof<R...> const & rhs) noexcept
	{
		return (hash_t)lhs >= (hash_t)rhs;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace std
{
	template <class ... T> struct equal_to<_ML typeof<T...>>
	{
		using type = _ML typeof<T...>;

		ML_NODISCARD constexpr bool operator()(type const & lhs, type const & rhs) const
		{
			return lhs == rhs;
		}
	};

	template <class ... T> struct hash<_ML typeof<T...>>
	{
		using type = _ML typeof<T...>;

		ML_NODISCARD constexpr size_t operator()(type const & value) const noexcept
		{
			return (_ML hash_t)value;
		}
	};
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_TYPE_INFO_HPP_