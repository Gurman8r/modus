#ifndef _ML_PYTHON_HPP_
#define _ML_PYTHON_HPP_

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <modus_core/Export.hpp>
#include <modus_core/detail/Matrix.hpp>
#include <modus_core/detail/Events.hpp>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// PYTHON

#ifndef HAVE_SNPRINTF
#define HAVE_SNPRINTF 1
#endif

#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/iostream.h>

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
	namespace py = pybind11;

	inline int32_t Python_DoFile(cstring path) noexcept
	{
		return PyRun_AnyFileEx(std::fopen(path, "r"), path, true);
	}

	inline int32_t Python_DoFile(ds::string const & path) noexcept
	{
		return Python_DoFile(path.c_str());
	}

	inline int32_t Python_DoFile(fs::path const & path) noexcept
	{
		auto const str{ path.string() };
		return Python_DoFile(str.c_str());
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_PYTHON_HPP_