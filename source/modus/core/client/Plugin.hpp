#ifndef _ML_PLUGIN_HPP_
#define _ML_PLUGIN_HPP_

#include <core/client/ClientContext.hpp>

#ifndef ML_PLUGIN_API
#define ML_PLUGIN_API ML_API_EXPORT
#endif

namespace ml
{
	struct plugin_manager;

	ML_decl_handle(plugin_id);

	struct ML_CORE_API plugin : client_object<plugin>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit plugin(plugin_manager * manager, void * user = nullptr) noexcept;

		virtual ~plugin() noexcept override = default;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		virtual void on_event(event &&) override = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_manager() const noexcept -> plugin_manager * { return m_manager; }

		ML_NODISCARD auto get_user_pointer() const noexcept -> void * { return m_userptr; }

		void * set_user_pointer(void * value) noexcept { return m_userptr = value; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager * const	m_manager	; // manager pointer
		void *					m_userptr	; // user pointer

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_PLUGIN_HPP_