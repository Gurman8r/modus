#ifndef _ML_PERFORMANCE_HPP_
#define _ML_PERFORMANCE_HPP_

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	// fps tracker
	struct ML_NODISCARD fps_tracker final : non_copyable, trackable
	{
		float32				value; // 
		float32				accum; // 
		size_t				index; // 
		ds::list<float32>	times; // 

		fps_tracker(size_t count = 120, pmr::polymorphic_allocator<byte> alloc = {}) noexcept
			: value{}, accum{}, index{}, times{ count, 0.f, alloc }
		{
		}

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