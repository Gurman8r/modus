#ifndef _ML_DEBUG_HPP_
#define _ML_DEBUG_HPP_

#include <modus_core/detail/StringUtility.hpp>

#define _ML_DEBUG _ML debug::

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// good message
#ifndef ML_DEBUG_MSG_SUCCESS
#define ML_DEBUG_MSG_SUCCESS "[info] "
#endif

// fail message
#ifndef ML_DEBUG_MSG_FAILURE
#define ML_DEBUG_MSG_FAILURE "[error] "
#endif

// warn message
#ifndef ML_DEBUG_MSG_WARNING
#define ML_DEBUG_MSG_WARNING "[warn] "
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// verify backend
#ifndef ML_IMPL_VERIFY
#define ML_IMPL_VERIFY ::_wassert
#endif

// verify extended
#define ML_verify_ex(expr, msg, file, line) \
	(void)((!!(expr)) || (ML_IMPL_VERIFY(ML_wide(msg), ML_wide(file), (unsigned)(line)), 0))

// verify message
#define ML_verify_msg(expr, msg) \
	ML_verify_ex(expr, msg, __FILE__, __LINE__)

// verify
#define ML_verify(expr) \
	ML_verify_msg(expr, ML_str(expr))

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// enable assertions
#ifndef ML_ENABLE_ASSERT
#define ML_ENABLE_ASSERT ML_is_debug
#endif

// assertions
#if ML_ENABLE_ASSERT
#define ML_assert(expr)	ML_verify(expr)
#define ML_check(expr)	(([](auto const & x) noexcept -> auto const & { ML_verify(x); return x; })(expr))
#else
#define ML_assert(expr)	ML_unused(expr)
#define ML_check(expr)	expr
#endif

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

	static struct ML_NODISCARD // I/O
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
		io.in.get().get();
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

	// SUCCESS | return 1
	ML_NODISCARD constexpr int32 good() noexcept { return 1; }

	// SUCCESS | return 1
	template <class Str
	> int32 good(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_SUCCESS << ML_forward(str) << "\n";

		return _ML_DEBUG good();
	}

	// SUCCESS | return 1
	template <class Fmt, class Arg0, class ... Args
	> int32 good(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG good
		(
			_ML util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// FAILURE | return 0
	ML_NODISCARD constexpr int32 fail() noexcept { return 0; }

	// FAILURE | return 0
	template <class Str
	> int32 fail(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_FAILURE << ML_forward(str) << "\n";

		return _ML_DEBUG fail();
	}

	// FAILURE | return 0
	template <class Fmt, class Arg0, class ... Args
	> int32 fail(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG fail
		(
			_ML util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// WARNING | return -1
	ML_NODISCARD constexpr int32 warn() noexcept { return -1; }

	// WARNING | return -1
	template <class Str
	> int32 warn(Str && str) noexcept
	{
		io.out.get() << ML_DEBUG_MSG_WARNING << ML_forward(str) << "\n";

		return _ML_DEBUG warn();
	}

	// WARNING | return -1
	template <class Fmt, class Arg0, class ... Args
	> int32 warn(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return _ML_DEBUG warn
		(
			_ML util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...)
		);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DEBUG_HPP_