#ifndef _ML_DEBUG_HPP_
#define _ML_DEBUG_HPP_

#include <modus_core/detail/StringUtility.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// info message
#ifndef ML_IMPL_DEBUG_INFO
#define ML_IMPL_DEBUG_INFO		"[info]"
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

// assert extended
#ifndef ML_assert_ext

#	ifdef ML_cc_msvc
#		define ML_assert_ext(expr, msg, file, line) (void)((!!(expr)) || (_wassert(ML_wide(msg), ML_wide(file), (unsigned)(line)), 0))

#	elif defined(assert)
#		define ML_assert_ext(expr, msg, file, line) assert(expr)

#	else
#		define ML_assert_ext(expr, msg, file, line) ((void)expr)

#	endif
#endif

// assert message
#define ML_assert_msg(expr, msg) \
	ML_assert_ext(expr, msg, __FILE__, __LINE__)

// assert
#define ML_assert(expr) \
	ML_assert_msg(expr, #expr)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// check message
#define ML_check_msg(expr, msg) ([](auto && x) noexcept {	\
		ML_assert_ext(x, msg, __FILE__, __LINE__);			\
		return ML_forward(x);								\
	})(expr)

// check
#define ML_check(expr) \
	ML_check_msg(expr, "CHECK FAILED: " ML_to_string(expr))

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

	inline int32_t clear(int32_t exit_code = EXIT_SUCCESS) noexcept
	{
#ifdef ML_os_windows
		std::system("cls");
#else
		std::system("clear");
#endif
		return exit_code;
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	inline int32_t pause(int32_t exit_code = EXIT_SUCCESS) noexcept
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

	constexpr int32_t info() noexcept { return 1; } // true

	template <class Fmt
	> int32_t info(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_INFO " " << ML_forward(fmt) << "\n";

		return debug::info();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32_t info(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::info(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32_t error() noexcept { return 0; } // false

	template <class Fmt
	> int32_t error(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_ERROR " " << ML_forward(fmt) << "\n";

		return debug::error();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32_t error(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::error(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	constexpr int32_t warning() noexcept { return -1; } // true

	template <class Fmt
	> int32_t warning(Fmt && fmt) noexcept
	{
		io.out << ML_IMPL_DEBUG_WARNING " " << ML_forward(fmt) << "\n";

		return debug::warning();
	}

	template <class Fmt, class Arg0, class ... Args
	> int32_t warning(Fmt && fmt, Arg0 && arg0, Args && ... args) noexcept
	{
		return debug::warning(util::format(ML_forward(fmt), ML_forward(arg0), ML_forward(args)...));
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_DEBUG_HPP_