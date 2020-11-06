#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : runtime_object<application>, loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		explicit application(runtime_context * const ctx) noexcept;

		virtual ~application() noexcept override;

		virtual void on_event(event &&) = 0;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		ML_NODISCARD auto get_plugins() noexcept -> plugin_manager & { return m_plugins; }

		ML_NODISCARD auto get_plugins() const noexcept -> plugin_manager const & { return m_plugins; }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager m_plugins; // plugin manager

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	};
}

// global application
namespace ml::globals
{
	ML_decl_global(application) get() noexcept;

	ML_decl_global(application) set(application * value) noexcept;
}

#endif // !_ML_MAIN_LOOP_HPP_