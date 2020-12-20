#ifndef _ML_MONITOR_HPP_
#define _ML_MONITOR_HPP_

#include <modus_core/system/Platform.hpp>
#include <modus_core/window/VideoMode.hpp>

namespace ml
{
	// monitor (WIP)
	struct ML_CORE_API monitor : trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		virtual ~monitor() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual ds::string get_name() const = 0;

		ML_NODISCARD virtual monitor_handle get_handle() const = 0;

		ML_NODISCARD virtual void * get_user_pointer() const = 0;

		virtual void set_user_pointer(void * value) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual vec2i get_dimensions() const = 0;

		ML_NODISCARD virtual vec2f get_content_scale() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual video_mode const & get_current_mode() const = 0;

		ML_NODISCARD virtual ds::list<video_mode> const & get_modes() const = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static monitor const & get_primary() noexcept;

		ML_NODISCARD static ds::list<monitor> const & get_monitors() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MONITOR_HPP_