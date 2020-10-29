#ifndef _ML_POINTER_HPP_
#define _ML_POINTER_HPP_

#include <modus_core/Common.hpp>

namespace ml
{
	// no delete
	struct no_delete final
	{
		template <class T
		> void operator()(T *) const noexcept {}
	};

	// default delete
	template <class ... T> struct default_delete;
}

// pointer types
namespace ml::ds
{
	// dumb pointer
	template <class T
	> ML_alias dumb = std::add_pointer_t<T>;

	// shared pointer
	template <class T
	> ML_alias shared = std::shared_ptr<T>;

	// weak pointer
	template <class T
	> ML_alias weak = std::weak_ptr<T>;

	// unique pointer
	template <class T, class Dx = default_delete<T>
	> ML_alias unique = std::unique_ptr<T, Dx>;

	// non-deleting pointer
	template <class T
	> ML_alias manual = std::unique_ptr<T, no_delete>;
}

#endif // !_ML_POINTER_HPP_