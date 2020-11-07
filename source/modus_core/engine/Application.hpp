#ifndef _ML_MAIN_LOOP_HPP_
#define _ML_MAIN_LOOP_HPP_

#include <modus_core/engine/PluginManager.hpp>

namespace ml
{
	// application
	struct ML_CORE_API application : engine_listener<application>, loop_system, non_copyable
	{
		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		explicit application(engine_context * const ctx) noexcept;

		virtual ~application() noexcept override;

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using engine_base::get_bus;

		using engine_base::get_context;

		using engine_base::get_database;

		using engine_base::get_io;

		using engine_base::get_main_loop;

		using engine_base::get_memory;

		using engine_base::get_window;

		ML_NODISCARD auto get_plugins() const noexcept { return const_cast<plugin_manager *>(&m_plugins); }

		/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	public:
		using event_callback = typename ds::method< void(event &&) >;

		ML_NODISCARD auto get_event_callback() const noexcept -> event_callback const &
		{
			return m_on_event;
		}

		template <class Fn, class ... Args
		> auto set_event_callback(Fn && fn, Args && ... args) noexcept -> event_callback &
		{
			return m_on_event = std::bind(ML_forward(fn), std::placeholders::_1, ML_forward(args)...);
		}

	protected:
		virtual void on_event(event && value) override // event_listener
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