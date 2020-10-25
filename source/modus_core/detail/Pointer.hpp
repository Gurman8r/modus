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
	template <class ...> struct default_delete;

	// unique pointer
	template <class T, class Dx = default_delete<T>
	> struct unique : public std::unique_ptr<T, Dx> {
		using std::unique_ptr<T, Dx>::unique_ptr;
	};

	// manual pointer ( no delete )
	template <class T
	> using manual = typename unique<T, no_delete>;

	// shared pointer
	template <class T
	> struct shared : public std::shared_ptr<T> {
		using std::shared_ptr<T>::shared_ptr;
	};

	// weak pointer
	template <class T
	> struct unown : public std::weak_ptr<T> {
		using std::weak_ptr<T>::weak_ptr;
	};
}

#endif // !_ML_POINTER_HPP_