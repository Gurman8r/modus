#ifndef _ML_PLATFORM_API_HPP_
#define _ML_PLATFORM_API_HPP_

// WIP

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	// platform api
	struct ML_CORE_API platform_api : non_copyable, trackable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		platform_api() noexcept;

		virtual ~platform_api() noexcept override;

	public:
		static platform_api * create() noexcept;

		static void destroy(platform_api * value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global platform_api
namespace ml::globals
{
	ML_decl_global(platform_api) get() noexcept;

	ML_decl_global(platform_api) set(platform_api * value) noexcept;
}

#endif // !_ML_PLATFORM_API_HPP_