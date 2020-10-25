#ifndef _ML_STRING_HPP_
#define _ML_STRING_HPP_

#include <modus_core/Common.hpp>

// STRING TEMPLATE
#define ML_STRING_TEMPLATE(Ch, Tr, Al)			\
	class Ch = char,							\
	class Tr = std::char_traits<Ch>,			\
	class Al = pmr::polymorphic_allocator<Ch>

// STRING
namespace ml::ds
{
	template <ML_STRING_TEMPLATE(Ch, Tr, Al)
	> ML_alias basic_string = typename std::basic_string
	<
		Ch, Tr, Al
	>;

	ML_alias string = typename basic_string<char>;

	ML_alias wstring = typename basic_string<wchar_t>;

	ML_alias u16string = typename basic_string<char16_t>;
	
	ML_alias u32string = typename basic_string<char32_t>;
}

// STRINGSTREAM
namespace ml::ds
{
	template <ML_STRING_TEMPLATE(Ch, Tr, Al)
	> ML_alias basic_stringstream = typename std::basic_stringstream
	<
		Ch, Tr, Al
	>;

	ML_alias stringstream = typename basic_stringstream<char>;

	ML_alias wstringstream = typename basic_stringstream<char>;
}

#endif // !_ML_STRING_HPP_