#ifndef _ML_RENDERER_HPP_
#define _ML_RENDERER_HPP_

#include <modus_core/graphics/RenderCommand.hpp>

namespace ml
{
	struct ML_CORE_API renderer final
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		renderer(allocator_type alloc = {}) noexcept;

		renderer(context_settings const & settings, allocator_type alloc = {}) noexcept;

		~renderer();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		bool initialize(context_settings const & settings, allocator_type alloc = {});

		void finalize();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_device() const noexcept -> gfx::render_device *
		{
			return m_device.get();
		}

		ML_NODISCARD auto get_context() const noexcept -> ds::ref<gfx::render_context> const &
		{
			return ML_check(m_device.get())->get_active_context();
		}

		void set_device(gfx::render_device * value) noexcept
		{
			m_device.reset(value);
		}

		void set_context(ds::ref<gfx::render_context> const & value) noexcept
		{
			ML_check(m_device.get())->set_active_context(value);
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		ds::scary<gfx::render_device> m_device; // render device
	};
}

#endif // !_ML_RENDERER_HPP_