#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <modus_core/system/EventSystem.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml { struct plugin_manager; } // plugin manager

namespace ml
{
	// core plugin
	template <class Manager> struct core_plugin : non_copyable, trackable, event_listener
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using allocator_type = typename pmr::polymorphic_allocator<byte>;

		ML_NODISCARD auto get_plugin_manager() const noexcept -> Manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void set_user_pointer(void * value) noexcept { m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		core_plugin(Manager * manager, void * userptr = nullptr)
			: event_listener{ manager->get_bus() }
			, m_manager		{ manager }
			, m_userptr		{ userptr }
		{
		}

		virtual ~core_plugin() noexcept override = default;

		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		Manager * const	m_manager	; // manager pointer
		void *			m_userptr	; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

namespace ml
{
	// plugin
	struct ML_CORE_API native_plugin : core_plugin<plugin_manager>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		using core_plugin<plugin_manager>::core_plugin;

		virtual ~native_plugin() noexcept override = default;

		virtual void on_event(event const &) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_