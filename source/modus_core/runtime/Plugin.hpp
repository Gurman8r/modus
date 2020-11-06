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

	public:
		using runtime_base = runtime_listener<plugin>;

		ML_NODISCARD auto get_app() const noexcept { return m_app; }

		using runtime_base::get_bus;

		using runtime_base::get_context;

		using runtime_base::get_db;

		using runtime_base::get_io;

		using runtime_base::get_loopsys;

		ML_NODISCARD auto get_manager() const noexcept { return m_manager; }
		
		using runtime_base::get_memory;

		using runtime_base::get_window;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD void * get_user_pointer() const noexcept { return m_userptr; }

		void * set_user_pointer(void * value) noexcept { return m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend plugin_manager;

		explicit plugin(plugin_manager * manager, void * userptr) noexcept;

		virtual ~plugin() noexcept override = default;

		using runtime_base::on_event; // handle event

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const		m_app		; // application
		plugin_manager * const	m_manager	; // manager
		void *					m_userptr	; // user

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_