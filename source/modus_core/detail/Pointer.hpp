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
	// shared pointer
	template <class Type
	> ML_alias ref = std::shared_ptr
	<
		Type
	>;

	// weak pointer
	template <class Type
	> ML_alias unown = std::weak_ptr
	<
		Type
	>;

	// unique pointer
	template <class Type, class Dx = default_delete<Type>
	> ML_alias scoped = std::unique_ptr
	<
		Type, Dx
	>;

	// non-deleting pointer
	template <class Type
	> ML_alias manual = std::unique_ptr
	<
		Type, no_delete
	>;
}

#endif // !_ML_POINTER_HPP_