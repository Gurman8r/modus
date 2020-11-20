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

namespace ml { namespace py = pybind11; } // ml::py::

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif // !_ML_PYTHON_HPP_