#ifndef _ML_CURSOR_HPP_
#define _ML_CURSOR_HPP_

#include <modus_core/detail/Memory.hpp>

namespace ml
{
	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	ML_decl_handle(cursor_handle); // cursor handle

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cursor mode
	enum cursor_mode_ : int32
	{
		cursor_mode_normal		, // normal
		cursor_mode_hidden		, // hidden
		cursor_mode_disabled	, // disabled
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cursor shape
	enum cursor_shape_ : int32
	{
		cursor_shape_arrow			, // arrow
		cursor_shape_ibeam			, // ibeam
		cursor_shape_crosshair		, // crosshair
		cursor_shape_pointing_hand	, // pointing_hand
		cursor_shape_ew				, // ew
		cursor_shape_ns				, // ns
		cursor_shape_nesw			, // nesw
		cursor_shape_nwse			, // nwse
		cursor_shape_resize_all		, // resize_all
		cursor_shape_not_allowed	, // not_allowed
		cursor_shape_hresize		, // hresize
		cursor_shape_vresize		, // vresize
		cursor_shape_hand			, // hand
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	// cursor (WIP)
	struct ML_CORE_API cursor final : non_copyable, trackable
	{
	public:
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~cursor() noexcept override { if (m_handle) { destroy(m_handle); } }

		cursor(cursor_handle value) noexcept : m_handle{ value } {}

		cursor(cursor_shape_ shape) noexcept
			: cursor{ create_standard(shape) } {}

		cursor(size_t w, size_t h, byte const * p, int32 x = 0, int32 y = 0) noexcept
			: cursor{ create_custom(w, h, p, x, y) } {}

		cursor() noexcept : cursor{ nullptr } {}

		cursor(cursor && other) noexcept : cursor{} { this->swap(std::move(other)); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		cursor & operator=(cursor && other) noexcept
		{
			this->swap(std::move(other));
			return (*this);
		}

		void swap(cursor & other) noexcept
		{
			if (this != std::addressof(other))
			{
				std::swap(m_handle, other.m_handle);
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_handle() const noexcept -> cursor_handle { return m_handle; }

		ML_NODISCARD static cursor_handle create_custom(size_t w, size_t h, byte const * p, int32 x = 0, int32 y = 0) noexcept;

		ML_NODISCARD static cursor_handle create_standard(cursor_shape_ shape) noexcept;

		static void destroy(cursor_handle value) noexcept;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		cursor_handle m_handle;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
}

#endif // !_ML_CURSOR_HPP_