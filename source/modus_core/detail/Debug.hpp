#ifndef _ML_DEBUG_HPP_
#define _ML_DEBUG_HPP_

#include <modus_core/detail/StringUtility.hpp>

#define _ML_DEBUG _ML debug::

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// success message
#ifndef ML_DEBUG_MSG_SUCCESS
#define ML_DEBUG_MSG_SUCCESS "[ok] "
#endif

// failure message
#ifndef ML_DEBUG_MSG_FAILURE
#define ML_DEBUG_MSG_FAILURE "[error] "
#endif

// warning message
#ifndef ML_DEBUG_MSG_WARNING
#define ML_DEBUG_MSG_WARNING "[warning] "
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// assert implementation
#ifndef ML_IMPL_ASSERT
#define ML_IMPL_ASSERT ::_wassert
#endif

// assert extended
#define ML_assert_ext(expr, msg, file, line) \
	(void)((!!(expr)) || (ML_IMPL_ASSERT(ML_wide(msg), ML_wide(file), (unsigned)(line)), 0))

// assert
#define ML_assert(expr) \
	ML_assert_ext(expr, ML_to_string(expr), __FILE__, __LINE__)

// check
#define ML_check(expr)												\
	([](auto const & x) noexcept -> auto const &					\
	{																\
		ML_assert_ext(x, ML_to_string(expr), __FILE__, __LINE__);	\
		return x;													\
	}																\
	)(expr)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// breakpoint
#ifndef ML_breakpoint

#	if defined(ML_cc_msvc)
#		define ML_breakpoint()	::__debugbreak()

#	elif defined(ML_cc_clang)
#		define ML_breakpoint()	::__builtin_debugtrap()

#	else
#		define ML_breakpoint()	::raise(SIGTRAP)

#	endif
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

namespace ml::debug
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	static struct ML_NODISCARD // io
	{
		std::reference_wrapper<std::ostream>	out		{	std::cout	};
		std::reference_wrapper<std::ostream>	err		{	std::cerr	};
		std::reference_wrapper<std::istream>	in		{	std::cin	};

		std::reference_wrapper<std::wostream>	wout	{	std::wcout	};
		std::reference_wrapper<std::wostream>	werr	{	std::wcerr	};
		std::reference_wrapper<std::wistream>	win		{	std::wcin	};
	}
	io;

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline int32 clear(int32 exit_code = EXIT_SUCCESS) noexcept
	{
#ifdef ML_os_windows
		std::system("cls");
#else
		std::system("clear");
#endif
		return exit_code;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline int32 pause(int32 exit_code = EXIT_SUCCESS) noexcept
	{
#ifdef ML_os_windows
		std::system("pause");
#else
		(io.in.get()).get();
#endif
		return exit_code;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Str
	> auto puts(Str && str) noexcept -> std::ostream &
	{
		return io.out.get() << ML_forward(str) << "\n";
	}

	template <class Fmt, class Arg0, class ... Args
	> auto puts(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept -> std::ostream &
	{
		return _ML_DEBUG puts
		(
			util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 success() noexcept { return 1; } // true

	template <class Str
	> int32 success(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_SUCCESS << ML_forward(str) << "\n";

		return _ML_DEBUG success();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 success(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG success
		(
			util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 failure() noexcept { return 0; } // false

	template <class Str
	> int32 failure(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_FAILURE << ML_forward(str) << "\n";

		return _ML_DEBUG failure();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 failure(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG failure
		(
			util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 warning() noexcept { return -1; } // true

	template <class Str
	> int32 warning(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_WARNING << ML_forward(str) << "\n";

		return _ML_DEBUG warning();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 warning(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG warning
		(
			util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DEBUG_HPP_