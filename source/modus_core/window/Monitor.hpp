#ifndef _ML_MONITOR_HPP_
#define _ML_MONITOR_HPP_

// WIP

#include <modus_core/window/VideoMode.hpp>

namespace ml
{
	// monitor
	struct monitor final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		monitor_handle			ID				; // 
		string					name			; // 
		void *					userptr			; // 
		vec2i					size			; // 
		vec2f					content_scale	; // 
		video_mode				current_mode	; // 
		list<video_mode>		modes			; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_MONITOR_HPP_