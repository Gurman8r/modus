#ifndef _ML_CLIENT_RUNTIME_HPP_
#define _ML_CLIENT_RUNTIME_HPP_

#include <modus_core/client/PluginManager.hpp>

namespace ml
{
	struct ML_CORE_API client_runtime final : client_object<client_runtime>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit client_runtime(client_context * context);

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		~client_runtime() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t idle();

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_event(event && value) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		bool			m_running	; // 
		plugin_manager	m_plugins	; // 

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

#endif // !_ML_CLIENT_RUNTIME_HPP_