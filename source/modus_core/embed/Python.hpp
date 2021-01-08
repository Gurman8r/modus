#ifndef _ML_PYTHON_HPP_
#define _ML_PYTHON_HPP_

#include <modus_core/detail/Matrix.hpp>
#include <modus_core/system/Memory.hpp>
#include <modus_core/detail/Timer.hpp>

#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF 1
#endif

#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

namespace pybind11
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// type casters
	namespace detail
	{
		// array caster
		template <class T, size_t N
		> struct type_caster<_ML array<T, N>>
			: array_caster<_ML array<T, N>, T, false, N> {};

		// matrix caster
		template <class T, size_t W, size_t H
		> struct type_caster<_ML matrix<T, W, H>>
			: array_caster<_ML matrix<T, W, H>, T, false, W * H> {};
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// to json
	static void to_json(_ML json & j, handle const & v)
	{
		j = _ML json::parse((std::string)(str)module::import("json").attr("dumps")(v));
	}

	// from json
	static void from_json(_ML json const & j, handle & v)
	{
		v = module::import("json").attr("loads")(j.dump());
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// eval file
	template <eval_mode Mode = eval_statements
	> object eval_file(std::filesystem::path const & path, object global = globals(), object local = {})
	{
		return ::pybind11::eval_file<Mode>((str)path.string(), global, local);
	}

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

namespace ml
{
	namespace py = pybind11;

	// is initialized
	ML_NODISCARD inline bool is_interpreter_initialized()
	{
		return Py_IsInitialized();
	}

	// initialize
	ML_NODISCARD inline bool initialize_interpreter(fs::path const & name, fs::path const & home)
	{
		ML_assert(!name.empty() && !home.empty());
		if (Py_IsInitialized()) { return false; }
		PyObject_SetArenaAllocator(std::invoke([&al = PyObjectArenaAllocator{}]() noexcept
		{
			al.ctx = pmr::get_default_resource();
			al.alloc = [](auto mres, size_t s) { return ((pmr::memory_resource *)mres)->allocate(s); };
			al.free = [](auto mres, void * p, size_t s) { return ((pmr::memory_resource *)mres)->deallocate(p, s); };
			return &al;
		}));
		Py_SetProgramName(name.c_str());
		Py_SetPythonHome(home.c_str());
		Py_InitializeEx(1);
		return Py_IsInitialized();
	}

	// finalize
	ML_NODISCARD inline bool finalize_interpreter()
	{
		return Py_IsInitialized() && (Py_FinalizeEx() == EXIT_SUCCESS);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_PYTHON_HPP_