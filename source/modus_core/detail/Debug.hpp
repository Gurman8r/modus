#ifndef _ML_DEBUG_HPP_
#define _ML_DEBUG_HPP_

#include <modus_core/detail/StringUtility.hpp>

#define _ML_DEBUG _ML debug::

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// good message
#ifndef ML_IMPL_DEBUG_INFO
#define ML_IMPL_DEBUG_INFO		"[good]"
#endif

// error message
#ifndef ML_IMPL_DEBUG_ERROR
#define ML_IMPL_DEBUG_ERROR		"[error]"
#endif

// warning message
#ifndef ML_IMPL_DEBUG_WARNING
#define ML_IMPL_DEBUG_WARNING	"[warn]"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// wassert
#ifndef ML_IMPL_WASSERT
#define ML_IMPL_WASSERT _CSTD _wassert
#endif

// assert extended
#define ML_assert_ext(expr, msg, file, line) \
	(void)((!!(expr)) || (ML_IMPL_WASSERT(ML_wide(msg), ML_wide(file), (unsigned)(line)), 0))

// assert message
#define ML_assert_msg(expr, msg) \
	ML_assert_ext(expr, msg, __FILE__, __LINE__)

// assert
#define ML_assert(expr) \
	ML_assert_msg(expr, #expr)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// check message
#define ML_check_msg(expr, msg) ([](auto const & x) noexcept -> auto const &	\
	{																			\
		ML_assert_ext(x, msg, __FILE__, __LINE__);								\
		return x;																\
	}																			\
	)(expr)

// check
#define ML_check(expr) \
	ML_check_msg(expr, "CHECK: " ML_to_string(expr))

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

	static struct // io
	{
		std::ostream & out	{ std::cout };
		std::ostream & err	{ std::cerr };
		std::istream & in	{ std::cin };
	}
	const io;

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
		io.in.get();
#endif
		return exit_code;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	template <class Fmt
	> auto & puts(Fmt && fmt) noexcept
	{
		return io.out << ML_forward(fmt) << "\n";
	}

	template <class Fmt, class Arg0, class ... Args
	> auto & puts(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::puts(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 good() noexcept { return 1; } // true

	template <class Fmt
	> int32 good(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_INFO " " << ML_forward(fmt) << "\n";

		return debug::good();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 good(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::good(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 error() noexcept { return 0; } // false

	template <class Fmt
	> int32 error(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_ERROR " " << ML_forward(fmt) << "\n";

		return debug::error();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 error(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::error(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32 warning() noexcept { return -1; } // true

	template <class Fmt
	> int32 warning(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_WARNING " " << ML_forward(fmt) << "\n";

		return debug::warning();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32 warning(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::warning(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DEBUG_HPP_