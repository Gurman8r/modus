#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <modus_core/detail/Events.hpp>
#include <modus_core/engine/Object.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	// plugin manager
	struct plugin_manager;

	// plugin id
	ML_decl_handle(plugin_id);

	// base plugin
	struct ML_CORE_API plugin : non_copyable, trackable, core_object
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		static constexpr plugin_id null{ nullptr };

		ML_NODISCARD auto get_plugin_manager() const noexcept -> plugin_manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		auto set_user_pointer(void * value) noexcept -> void * { return m_userptr = value; }

	protected:
		friend plugin_manager;

		plugin(plugin_manager * manager, void * userptr = nullptr);

		virtual ~plugin() override;

		virtual void on_event(event const &) override = 0;

	private:
		plugin_manager * const	m_manager;
		void *					m_userptr;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_