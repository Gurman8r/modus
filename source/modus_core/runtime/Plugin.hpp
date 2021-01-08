#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <modus_core/system/EventSystem.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	struct plugin_manager; // plugin manager

	// plugin
	struct ML_CORE_API plugin : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		ML_NODISCARD auto get_allocator() const noexcept -> allocator_type { return m_alloc; }

		ML_NODISCARD auto get_plugin_manager() const noexcept -> plugin_manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend plugin_manager;

		plugin(plugin_manager * manager, void * userptr = nullptr);

		virtual ~plugin() noexcept override;

		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager * const	m_manager	; // manager pointer
		allocator_type			m_alloc		; // allocator
		void *					m_userptr	; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_