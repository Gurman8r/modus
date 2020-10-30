#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <modus_core/runtime/Runtime.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	struct application;

	struct plugin_manager;

	ML_decl_handle(plugin_id);

	struct ML_CORE_API plugin : runtime_listener<plugin>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit plugin(plugin_manager * manager, void * userptr = nullptr);

		virtual ~plugin() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_application() const noexcept -> application * { return m_app; }

		ML_NODISCARD auto get_manager() const noexcept -> plugin_manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void * set_user_pointer(void * value) noexcept { return m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const		m_app		; // application
		plugin_manager * const	m_manager	; // manager
		void *					m_userptr	; // user

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_