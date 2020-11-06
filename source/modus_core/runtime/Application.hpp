#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : runtime_listener<application>, loop_system
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using runtime_base = runtime_listener<application>;

		using runtime_base::get_bus;

		using runtime_base::get_context;

		using runtime_base::get_db;

		using runtime_base::get_io;

		using runtime_base::get_loopsys;

		using runtime_base::get_memory;

		using runtime_base::get_window;

		ML_NODISCARD auto get_plugins() const noexcept { return const_cast<plugin_manager *>(&m_plugins); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		explicit application(runtime_context * const ctx) noexcept;

		virtual ~application() noexcept override;

		using runtime_base::on_event; // handle event

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		plugin_manager m_plugins;

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