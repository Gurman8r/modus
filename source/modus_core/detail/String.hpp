#ifndef _ML_STRING_HPP_
#define _ML_STRING_HPP_

#include <modus_core/detail/List.hpp>

// STRING TEMPLATE
#define ML_STRING_TEMPLATE(Ch, Tr, Al)					\
	class Ch	= char,									\
	class Tr	= _ML std::char_traits<Ch>,				\
	class Al	= _ML pmr::polymorphic_allocator<Ch>

// BASIC STRING TEMPLATE
#define ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)		\
	class Ch	= char,									\
	class Tr	= _ML std::char_traits<Ch>,				\
	class Al	= _ML pmr::polymorphic_allocator<Ch>,	\
	class Str	= _ML basic_string<Ch, Tr, Al>

// STRING
namespace ml
{
	template <ML_STRING_TEMPLATE(Ch, Tr, Al)
	> ML_alias basic_string = typename std::basic_string // basic_string
	<
		Ch, Tr, Al
	>;

	ML_alias string = typename basic_string<char>; // string

	ML_alias wstring = typename basic_string<wchar_t>; // wstring

	ML_alias u16string = typename basic_string<char16_t>; // u16string
	
	ML_alias u32string = typename basic_string<char32_t>; // u32string
}

// STRINGSTREAM
namespace ml
{
	template <ML_STRING_TEMPLATE(Ch, Tr, Al)
	> ML_alias basic_stringstream = typename std::basic_stringstream // basic_stringstream
	<
		Ch, Tr, Al
	>;

	ML_alias stringstream = typename basic_stringstream<char>; // stringstream

	ML_alias wstringstream = typename basic_stringstream<wchar_t>; // wstringstream
}

#endif // !_ML_STRING_HPP_