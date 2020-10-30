#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : runtime_listener<application>, loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		explicit application(runtime_api * api);

		virtual ~application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		ML_NODISCARD auto get_plugins() const noexcept -> plugin_manager const & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		virtual void on_event(event &&) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager m_plugins;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global application
namespace ml::globals
{
	template <> ML_NODISCARD ML_CORE_API application * get() noexcept;

	template <> ML_CORE_API application * set(application * value) noexcept;
}

#endif // !_ML_MAIN_LOOP_HPP_