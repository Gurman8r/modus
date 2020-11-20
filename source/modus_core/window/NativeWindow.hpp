#ifndef _ML_NATIVE_WINDOW_HPP_
#define _ML_NATIVE_WINDOW_HPP_

#include <modus_core/window/BaseWindow.hpp>

namespace ml
{
	// native window
	struct ML_CORE_API native_window : frontend_window
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using frontend_window::allocator_type;

		native_window(allocator_type alloc = {}) noexcept;

		native_window(
			ds::string			const & title,
			video_mode			const & vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			void *						userptr	= nullptr,
			allocator_type				alloc	= {}) noexcept;
		
		virtual ~native_window() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD virtual bool open(
			ds::string			const &	title,
			video_mode			const &	vm		= {},
			context_settings	const & cs		= {},
			window_hints_				hints	= window_hints_default,
			void *						userptr	= nullptr
		) override;
		
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_NATIVE_WINDOW_HPP_