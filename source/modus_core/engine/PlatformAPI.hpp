#ifndef _ML_PLATFORM_API_HPP_
#define _ML_PLATFORM_API_HPP_

// WIP

#include <modus_core/engine/Object.hpp>

namespace ml
{
	// base platform api
	struct ML_CORE_API platform_api : non_copyable, trackable, core_object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		explicit platform_api(event_bus * bus) noexcept : core_object{ bus } {}

		virtual ~platform_api() noexcept override = default;

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte_t>;

		static platform_api * create(event_bus * bus, allocator_type alloc = {}) noexcept;

		static void destroy(platform_api * value = nullptr) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	// make platform api
	template <class ... Args
	> ML_NODISCARD auto make_platform_api(Args && ... args) noexcept
	{
		return platform_api::create(ML_forward(args)...);
	}

	// destroy platform api
	inline void destroy_platform_api(platform_api * value) noexcept
	{
		platform_api::destroy(value);
	}
}

// global platform_api
namespace ml::globals
{
	ML_decl_global(platform_api) get() noexcept;

	ML_decl_global(platform_api) set(platform_api * value) noexcept;
}

#endif // !_ML_PLATFORM_API_HPP_