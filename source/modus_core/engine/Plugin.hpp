#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <modus_core/engine/Engine.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	struct application;

	struct plugin_manager;

	ML_decl_handle(plugin_id);

	struct ML_CORE_API plugin : engine_listener<plugin>, trackable, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		friend plugin_manager;

		explicit plugin(plugin_manager * manager, void * userptr = nullptr) noexcept;

		virtual ~plugin() noexcept override = default;

		using engine_base::on_event; // event_listener

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_app() const noexcept { return m_app; }

		using engine_base::get_bus;

		using engine_base::get_context;

		using engine_base::get_database;

		using engine_base::get_io;

		using engine_base::get_main_loop;

		ML_NODISCARD auto get_manager() const noexcept { return m_manager; }
		
		using engine_base::get_memory;

		using engine_base::get_window;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD void * get_user_pointer() const noexcept { return m_userptr; }

		void * set_user_pointer(void * value) noexcept { return m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		application * const		m_app		; // application
		plugin_manager * const	m_manager	; // manager
		void *					m_userptr	; // user

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_