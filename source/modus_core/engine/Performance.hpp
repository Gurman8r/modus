#ifndef _ML_PERFORMANCE_HPP_
#define _ML_PERFORMANCE_HPP_

#include <modus_core/detail/Array.hpp>
#include <modus_core/detail/Memory.hpp>

namespace ml
{
	// fps tracker
	template <size_t N = 120
	> struct ML_NODISCARD fps_tracker final : non_copyable, trackable
	{
		float32					value{}; // 
		float32					accum{}; // 
		size_t					index{}; // 
		ds::array<float32, N>	times{}; // 

		fps_tracker() noexcept = default;

		void operator()(float32 dt) noexcept
		{
			accum += dt - times[index];
			times[index] = dt;
			index = (index + 1) % times.size();
			value = (0.f < accum) ? (1.f / (accum / (float32)times.size())) : FLT_MAX;
		}
	};
}

#endif // !_ML_PERFORMANCE_HPP_