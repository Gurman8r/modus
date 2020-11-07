#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/runtime/PluginManager.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : runtime_listener<application>, loop_system, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		explicit application(runtime_context * const ctx) noexcept;

		virtual ~application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
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
		using loop_system::set_loop_condition;

		using loop_system::set_enter_callback;

		using loop_system::set_exit_callback;

		using loop_system::set_idle_callback;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	protected:
		using event_callback = typename ds::method<void(event &&)>;

		ML_NODISCARD auto get_event_callback() const noexcept -> event_callback const &
		{
			return m_on_event;
		}

		template <class Fn, class ... Args
		> auto set_event_callback(Fn && fn, Args && ... args) noexcept -> event_callback &
		{
			return loop_system::bind
			(
				&application::m_on_event, this, ML_forward(fn), std::placeholders::_1, ML_forward(args)...
			);
		}

		// handle event
		virtual void on_event(event && value) override
		{
			loop_system::exec(&application::m_on_event, this, ML_forward(value));
		}

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	private:
		event_callback m_on_event	; // event callback
		plugin_manager m_plugins	; // plugin manager

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