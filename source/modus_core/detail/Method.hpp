#ifndef _ML_METHOD_HPP_
#define _ML_METHOD_HPP_

#include <modus_core/detail/Utility.hpp>

namespace ml
{
	// method
	template <class Signature
	> struct method : public std::function<Signature>
	{
		using std::function<Signature>::function;
	};

	// delegate
	template <class Signature
	> struct delegate : public list<method<Signature>>
	{
		using list<method<Signature>>::list;

		template <class ... Args
		> void operator()(Args && ... args) const noexcept
		{
			for (auto const & e : *this)
			{
				ML_check(e)(ML_forward(args)...);
			}
		}

		template <class Fn
		> auto & operator+=(Fn && value) noexcept
		{
			this->emplace_back(ML_forward(value));

			return (*this);
		}
	};
}

namespace ml::util
{
	// bind method and return its previous value
	template <class Clbk, class Fn, class ... Args
	> constexpr auto chain(Clbk & clbk, Fn && fn, Args && ... args) noexcept
	{
		auto const prev{ std::move(clbk) };
		
		if constexpr (0 < sizeof...(args))
		{
			clbk = std::bind(ML_forward(fn), ML_forward(args)...);
		}
		else
		{
			clbk = Clbk{ ML_forward(fn) };
		}
		
		return prev;
	}
}

#endif // !_ML_METHOD_HPP_