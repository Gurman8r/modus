#ifndef _ML_CLIENT_RUNTIME_HPP_
#define _ML_CLIENT_RUNTIME_HPP_

#include <core/client/PluginManager.hpp>

namespace ml
{
	struct ML_CORE_API client_runtime final : client_object<client_runtime>
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit client_runtime(client_context * context);

		~client_runtime() override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		void on_event(event &&) override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD int32_t process(std::function<void()> const & fn = {});

		template <class Fn, class Arg0, class ... Args
		> ML_NODISCARD int32_t process(Fn && fn, Arg0 && arg0, Args && ... args) noexcept
		{
			return this->process(std::bind(ML_forward(fn), ML_forward(arg0), ML_forward(args)...));
		}

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