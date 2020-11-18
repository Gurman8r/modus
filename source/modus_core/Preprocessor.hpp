#ifndef _ML_PREPROCESSOR_HPP_
#define _ML_PREPROCESSOR_HPP_

#include <modus_core/Config.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// concatenate implementation
#define ML_cat_impl(a, b)		a##b

// concatenate
#define ML_cat(a, b)			ML_cat_impl(a, b)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// crt wide string
#define ML_wide(str)			ML_cat(L, str)

// macro to string
#define ML_to_string(expr)		#expr

// macro contents to string
#define ML_stringify(expr)		ML_to_string(expr)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// anonymous expressions		_expr_#_
#if defined(__COUNTER__)
#	define ML_anon_ex(expr)		ML_cat(_, ML_cat(expr, ML_cat(_, ML_cat(__COUNTER__, _))))
#elif defined(__LINE__)
#	define ML_anon_ex(expr)		ML_cat(_, ML_cat(expr, ML_cat(_, ML_cat(__LINE__, _))))
#else
#	define ML_anon_ex(expr)		ML_cat(_, ML_cat(expr, _))
#endif

#define ML_anon					ML_anon_ex(anonymous) // anonymous variable

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_PREPROCESSOR_HPP_