#ifndef _ML_PYTHON_HPP_
#define _ML_PYTHON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/EventSystem.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PYTHON

#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF 1
#endif

#include <Python.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PYBIND11

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

namespace ml { namespace py = pybind11; } // ml::py

namespace pybind11
{
	namespace detail
	{
		// array caster
		template <class T, size_t N
		> struct type_caster<_ML ds::array<T, N>>
			: array_caster<_ML ds::array<T, N>, T, false, N> {};

		// matrix caster
		template <class T, size_t W, size_t H
		> struct type_caster<_ML ds::matrix<T, W, H>>
			: array_caster<_ML ds::matrix<T, W, H>, T, false, W * H> {};
	}

	static void to_json(_ML json & j, handle const & v)
	{
		j = _ML json::parse((std::string)(str)module::import("json").attr("dumps")(v));
	}

	static void from_json(_ML json const & j, handle & v)
	{
		v = module::import("json").attr("loads")(j.dump());
	}
}

namespace ml
{
	struct python_interpreter final : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <class ... Args
		> python_interpreter(Args && ... args) noexcept
		{
			ML_assert(begin_singleton<python_interpreter>(this));
			
			if constexpr (0 < sizeof...(args))
			{
				ML_assert(this->initialize(ML_forward(args)...));
			}
		}

		~python_interpreter() noexcept final
		{
			ML_assert(end_singleton<python_interpreter>(this));

			this->finalize();
		}

		python_interpreter(python_interpreter &&) noexcept = default;
		python_interpreter & operator=(python_interpreter &&) noexcept = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD bool is_initialized() const noexcept
		{
			return Py_IsInitialized();
		}

		bool initialize(fs::path const & name, fs::path const & home, pmr::memory_resource * resource = pmr::get_default_resource())
		{
			if (name.empty())		{ return debug::failure(); }
			if (home.empty())		{ return debug::failure(); }
			if (!resource)			{ return debug::failure(); }
			if (Py_IsInitialized())	{ return debug::failure(); }

			PyObject_SetArenaAllocator(std::invoke([&, &al = PyObjectArenaAllocator{}]()
			{
				al.ctx = resource;
				al.alloc = [](auto resource, size_t s) {
					return ((pmr::memory_resource *)resource)->allocate(s);
				};
				al.free = [](auto resource, void * p, size_t s) {
					return ((pmr::memory_resource *)resource)->deallocate(p, s);
				};
				return &al;
			}));
			Py_SetProgramName((m_program_name = name).c_str());
			Py_SetPythonHome((m_library_home = home).c_str());
			Py_InitializeEx(1);
			return Py_IsInitialized();
		}

		bool finalize()
		{
			return !Py_IsInitialized() && (Py_FinalizeEx() == EXIT_SUCCESS);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <py::eval_mode Mode = py::eval_expr, size_t N
		> py::object eval(const char (&expr)[N], py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval<Mode>(expr, global, local);
		}

		template <py::eval_mode Mode = py::eval_expr
		> py::object eval(py::str expr, py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval<Mode>(expr, global, local);
		}

		template <py::eval_mode Mode = py::eval_expr, ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
		> py::object eval(Str const & expr, py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval<Mode>(py::str{ expr.data(), expr.size() }, global, local);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <size_t N
		> void exec(const char(&expr)[N], py::object global = py::globals(), py::object local = {}) noexcept
		{
			py::exec(expr, global, local);
		}

		void exec(py::str expr, py::object global = py::globals(), py::object local = {}) noexcept
		{
			py::exec(expr, global, local);
		}

		template <ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
		> void exec(Str const & expr, py::object global = py::globals(), py::object local = {}) noexcept
		{
			py::exec(py::str{ expr.data(), expr.size() }, global, local);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		template <py::eval_mode Mode = py::eval_statements, size_t N
		> py::object eval_file(const char (&path)[N], py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval_file<Mode>(path, global, local);
		}

		template <py::eval_mode Mode = py::eval_statements
		> py::object eval_file(py::str path, py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval_file<Mode>(path, global, local);
		}

		template <py::eval_mode Mode = py::eval_statements, ML_BASIC_STRING_TEMPLATE(Ch, Tr, Al, Str)
		> py::object eval_file(Str const & path, py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval_file<Mode>(py::str{ path.data(), path.size() }, global, local);
		}

		template <py::eval_mode Mode = py::eval_statements
		> py::object eval_file(fs::path const & path, py::object global = py::globals(), py::object local = {}) noexcept
		{
			return py::eval_file<Mode>(path.string(), global, local);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		fs::path m_program_name{};
		fs::path m_library_home{};

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml::globals
{
	ML_decl_global(python_interpreter) get() noexcept;

	ML_decl_global(python_interpreter) set(python_interpreter * value) noexcept;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_PYTHON_HPP_