#ifndef _ML_COMMON_HPP_
#define _ML_COMMON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <Config.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (ML_has_cxx11 && ML_has_cxx14)
#	include <cassert>
#	include <chrono>
#	include <fstream>
#	include <functional>
#	include <iostream>
#	include <sstream>
#	include <vector>
#endif

#if (ML_has_cxx17)
#	include <any>
#	include <filesystem>
#	include <memory_resource>
#	include <optional>
#	include <string_view>
#	include <variant>
#endif

#if (ML_has_cxx20)
#	include <compare>
#	include <concepts>
#	include <format>
#	include <numbers>
#	include <ranges>
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// concatenate implementation
#define ML_cat_impl(a, b)		a##b

// concatenate
#define ML_cat(a, b)			ML_cat_impl(a, b)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// macro to string
#define ML_to_string(expr)		#expr

// macro contents to string
#define ML_stringify(expr)		ML_to_string(expr)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// anonymous expressions		_expr_#_
#if defined(__COUNTER__)
#	define ML_make_anon(expr)	ML_cat(_, ML_cat(expr, ML_cat(_, ML_cat(__COUNTER__, _))))
#elif defined(__LINE__)
#	define ML_make_anon(expr)	ML_cat(_, ML_cat(expr, ML_cat(_, ML_cat(__LINE__, _))))
#else
#	define ML_make_anon(expr)	ML_cat(_, ML_cat(expr, _))
#endif

#define ML_anon					ML_make_anon(anonymous) // anonymous variable

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// root namespace
#define _ML						::ml::

// global typedef
#define ML_alias				using

// fixed array size
#define ML_arraysize(arr)		(sizeof(arr) / sizeof(*arr))

// ternary comparison
#define ML_compare(lhs, rhs)	(((lhs) != (rhs)) ? (((lhs) < (rhs)) ? -1 : 1) : 0)

// automatic forward
#define ML_forward(expr)		std::forward<decltype(expr)>(expr)

// crt wide string
#define ML_wide(str)			ML_cat(L, str)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// declare handle
#define ML_decl_handle(name)	struct ML_cat(name, __) { _ML int32_t unused; }; \
								using name = typename ML_cat(name, __) *

// handle cast
#define ML_handle(type, value)	((type)(_ML intptr_t)(value))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml
{
	// standard namespaces
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace std		= ::std;
	namespace chrono	= std::chrono;
	namespace fs		= std::filesystem;
	namespace pmr		= std::pmr;
#if (ML_has_cxx20)
	namespace ranges	= std::ranges;
	namespace views		= std::ranges::views;
#endif

	// core types
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias	int8_t		= typename ML_int8		;
	ML_alias	int16_t		= typename ML_int16		;
	ML_alias	int32_t		= typename ML_int32		;
	ML_alias	int64_t		= typename ML_int64		;

	ML_alias	uint8_t		= typename ML_uint8		;
	ML_alias	uint16_t	= typename ML_uint16	;
	ML_alias	uint32_t	= typename ML_uint32	;
	ML_alias	uint64_t	= typename ML_uint64	;

	ML_alias	byte_t		= typename ML_byte		;
	ML_alias	char_t		= typename ML_char		;
	ML_alias	intmax_t	= typename ML_intmax	;
	ML_alias	uintmax_t	= typename ML_uintmax	;
	ML_alias	ulong_t		= typename ML_ulong		;

	ML_alias	float32_t	= typename ML_float32	;
	ML_alias	float64_t	= typename ML_float64	;
	ML_alias	float80_t	= typename ML_float80	;

	// helper types
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias	float_t		= typename float32_t	;
	ML_alias	hash_t		= typename uintmax_t	;
	ML_alias	intptr_t	= typename intmax_t		;
	ML_alias	ptrdiff_t	= typename intmax_t		;
	ML_alias	max_align_t = typename float64_t	;
	ML_alias	size_t		= typename uintmax_t	;

	// c-string types
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias	cstring		= typename char		const *	;
	ML_alias	cwstring	= typename wchar_t	const *	;
#if (ML_has_cxx20)
	ML_alias	c8string	= typename char8_t	const *	;
#endif
	ML_alias	c16string	= typename char16_t const *	;
	ML_alias	c32string	= typename char32_t const *	;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_COMMON_HPP_