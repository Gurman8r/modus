#ifndef _ML_METHOD_HPP_
#define _ML_METHOD_HPP_

// WIP

#include <modus_core/detail/Utility.hpp>

namespace ml::ds
{
	// method
	template <class Signature
	> struct method : public std::function<Signature>
	{
		using std::function<Signature>::function;
	};
}

#endif // !_ML_METHOD_HPP_