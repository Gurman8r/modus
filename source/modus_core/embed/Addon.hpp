#ifndef _ML_ADDON_HPP_
#define _ML_ADDON_HPP_

#include <modus_core/system/EventSystem.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	// addon manager
	struct addon_manager;

	// native addon
	struct ML_CORE_API addon : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		ML_NODISCARD auto get_addon_manager() const noexcept -> addon_manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend addon_manager;

		addon(addon_manager * manager, void * userptr = nullptr);

		virtual ~addon() noexcept override;

		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		addon_manager * const m_manager; // manager pointer

		void * m_userptr; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_ADDON_HPP_