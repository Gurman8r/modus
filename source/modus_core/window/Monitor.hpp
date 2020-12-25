#ifndef _ML_MONITOR_HPP_
#define _ML_MONITOR_HPP_

// WIP

#include <modus_core/window/VideoMode.hpp>

namespace ml
{
	// monitor handle
	ML_decl_handle(monitor_handle);

	// monitor
	struct ML_CORE_API monitor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		monitor_handle			ID				; // 
		ds::string				name			; // 
		void *					userptr			; // 
		vec2i					size			; // 
		vec2f					content_scale	; // 
		video_mode				current_mode	; // 
		ds::list<video_mode>	modes			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD static monitor const & get_primary() noexcept;

		ML_NODISCARD static ds::list<monitor> const & get_monitors() noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MONITOR_HPP_