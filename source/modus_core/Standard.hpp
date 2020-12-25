#ifndef _ML_STANDARD_HPP_
#define _ML_STANDARD_HPP_

#include <modus_core/Preprocessor.hpp>

static_assert(ML_has_cxx14);
#include <cassert>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <vector>

static_assert(ML_has_cxx17);
#include <any>
#include <filesystem>
#include <memory_resource>
#include <optional>
#include <string_view>
#include <variant>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	namespace std		= ::std;
	namespace chrono	= std::chrono;
	namespace fs		= std::filesystem;
	namespace pmr		= std::pmr;
#if (ML_has_cxx20)
	namespace ranges	= std::ranges;
	namespace views		= std::ranges::views;
#endif

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias	byte		= typename ML_byte		; // 

	ML_alias	int8		= typename ML_int8		; // 
	ML_alias	int16		= typename ML_int16		; // 
	ML_alias	int32		= typename ML_int32		; // 
	ML_alias	int64		= typename ML_int64		; // 

	ML_alias	uint8		= typename ML_uint8		; // 
	ML_alias	uint16		= typename ML_uint16	; // 
	ML_alias	uint32		= typename ML_uint32	; // 
	ML_alias	uint64		= typename ML_uint64	; // 

	ML_alias	float32		= typename ML_float32	; // 
	ML_alias	float64		= typename ML_float64	; // 
	ML_alias	float80		= typename ML_float80	; // 

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#if (ML_arch == 32)
	ML_alias	intmax_t	= typename int32		; // 
	ML_alias	uintmax_t	= typename uint32		; // 
#else
	ML_alias	intmax_t	= typename int64		; // 
	ML_alias	uintmax_t	= typename uint64		; // 
#endif
	
	ML_alias	double_t	= typename float64		; // 
	ML_alias	float_t		= typename float32		; // 
	ML_alias	hash_t		= typename uintmax_t	; // 
	ML_alias	intptr_t	= typename intmax_t		; // 
	ML_alias	ptrdiff_t	= typename intmax_t		; // 
	ML_alias	size_t		= typename uintmax_t	; // 
	ML_alias	max_align_t = typename float64		; // 

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_alias	cstring		= typename char		const *	; // 
	ML_alias	cwstring	= typename wchar_t	const *	; // 
#if (ML_has_cxx20)
	ML_alias	c8string	= typename char8_t	const *	; // 
#endif
	ML_alias	c16string	= typename char16_t const *	; // 
	ML_alias	c32string	= typename char32_t const *	; // 

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_STANDARD_HPP_